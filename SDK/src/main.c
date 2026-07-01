#include <stdio.h>

#include "xil_printf.h"
#include "xparameters.h"

#include "xaxidma.h"
#include "xsobel_accel.h"

#include "xil_cache.h"
#include "xtime_l.h"

#include "image.h"

XAxiDma AxiDma;
XSobel_accel Sobel;

// Output image buffer
u8 RxBuffer[IMG_SIZE] __attribute__((aligned(64)));

int DMA_Init(void);
int Sobel_Init(void);

int main()
{
    int Status;
    XTime tStart, tEnd;
    double Time_ms;


    xil_printf("\r\n");
    xil_printf("====================================\r\n");
    xil_printf("      Sobel Accelerator Demo\r\n");
    xil_printf("====================================\r\n");

    //--------------------------------------------------
    // DMA Init
    //--------------------------------------------------

    Status = DMA_Init();

    if(Status != XST_SUCCESS)
    {
        xil_printf("DMA Init Error!\r\n");
        return -1;
    }

    //--------------------------------------------------
    // Sobel Init
    //--------------------------------------------------

    Status = Sobel_Init();

    if(Status != XST_SUCCESS)
    {
        xil_printf("Sobel Init Error!\r\n");
        return -1;
    }

    //--------------------------------------------------
    // Configure Sobel
    //--------------------------------------------------

    XSobel_accel_Set_width(&Sobel, IMG_WIDTH);
    XSobel_accel_Set_height(&Sobel, IMG_HEIGHT);

    //--------------------------------------------------
    // Flush Cache
    //--------------------------------------------------

    Xil_DCacheFlushRange((UINTPTR)TxBuffer, IMG_SIZE);
    Xil_DCacheFlushRange((UINTPTR)RxBuffer, IMG_SIZE);

    //--------------------------------------------------
    // Start Sobel IP
    //--------------------------------------------------

    XSobel_accel_Start(&Sobel);

    //--------------------------------------------------
    // Start Timer
    //--------------------------------------------------

    XTime_GetTime(&tStart);

    //--------------------------------------------------
    // Start RX DMA
    //--------------------------------------------------

    Status = XAxiDma_SimpleTransfer(
                &AxiDma,
                (UINTPTR)RxBuffer,
                IMG_SIZE,
                XAXIDMA_DEVICE_TO_DMA);

    if(Status != XST_SUCCESS)
    {
        xil_printf("RX DMA Error!\r\n");
        return -1;
    }

    //--------------------------------------------------
    // Start TX DMA
    //--------------------------------------------------

    Status = XAxiDma_SimpleTransfer(
                &AxiDma,
                (UINTPTR)TxBuffer,
                IMG_SIZE,
                XAXIDMA_DMA_TO_DEVICE);

    if(Status != XST_SUCCESS)
    {
        xil_printf("TX DMA Error!\r\n");
        return -1;
    }

    //--------------------------------------------------
    // Wait DMA Finish
    //--------------------------------------------------

    while(XAxiDma_Busy(&AxiDma, XAXIDMA_DMA_TO_DEVICE));

    while(XAxiDma_Busy(&AxiDma, XAXIDMA_DEVICE_TO_DMA));

    //--------------------------------------------------
    // Wait Sobel Finish
    //--------------------------------------------------

    while(!XSobel_accel_IsDone(&Sobel));

    //--------------------------------------------------
    // Stop Timer
    //--------------------------------------------------

    XTime_GetTime(&tEnd);

    //--------------------------------------------------
    // Invalidate Cache
    //--------------------------------------------------

    Xil_DCacheInvalidateRange((UINTPTR)RxBuffer, IMG_SIZE);

    //--------------------------------------------------
    // Calculate Time
    //--------------------------------------------------

    Time_ms = (double)(2 * (tEnd - tStart)) /
              (XPAR_CPU_CORTEXA9_CORE_CLOCK_FREQ_HZ / 1000.0);

    xil_printf("\r\n");
    xil_printf("Processing Finished!\r\n");
    xil_printf("Clock Cycles : %llu\r\n", 2*(tEnd-tStart));
    xil_printf("Time         : %.3f ms\r\n", Time_ms);
    xil_printf("FPS          : %.2f\r\n", 1000.0/Time_ms);

    //--------------------------------------------------
    // Print First 20 Pixels
    //--------------------------------------------------

    xil_printf("\r\nFirst 20 Output Pixels:\r\n");

    for(int i=0;i<20;i++)
    {
        xil_printf("%3d ",RxBuffer[i]);
    }

    xil_printf("\r\n");


    while(1);

    return 0;
}

/******************************************************
 * DMA Initialization
 *****************************************************/
int DMA_Init(void)
{
    XAxiDma_Config *CfgPtr;

    CfgPtr = XAxiDma_LookupConfig(XPAR_AXIDMA_0_DEVICE_ID);

    if(CfgPtr == NULL)
    {
        xil_printf("ERROR: DMA Config Not Found!\r\n");
        return XST_FAILURE;
    }

    if(XAxiDma_CfgInitialize(&AxiDma,CfgPtr)!=XST_SUCCESS)
    {
        xil_printf("ERROR: DMA Init Failed!\r\n");
        return XST_FAILURE;
    }

    if(XAxiDma_HasSg(&AxiDma))
    {
        xil_printf("ERROR: DMA is SG Mode!\r\n");
        return XST_FAILURE;
    }

    XAxiDma_Reset(&AxiDma);

    while(!XAxiDma_ResetIsDone(&AxiDma));

    xil_printf("DMA Init OK\r\n");

    return XST_SUCCESS;
}

/******************************************************
 * Sobel Initialization
 *****************************************************/
int Sobel_Init(void)
{
    XSobel_accel_Config *CfgPtr;

    CfgPtr = XSobel_accel_LookupConfig(
                    XPAR_SOBEL_ACCEL_0_DEVICE_ID);

    if(CfgPtr == NULL)
    {
        xil_printf("ERROR: Sobel Config Not Found!\r\n");
        return XST_FAILURE;
    }

    if(XSobel_accel_CfgInitialize(
            &Sobel,
            CfgPtr)!=XST_SUCCESS)
    {
        xil_printf("ERROR: Sobel Init Failed!\r\n");
        return XST_FAILURE;
    }

    xil_printf("Sobel Init OK\r\n");

    return XST_SUCCESS;
}
