
/* -----------------------------------------------------------------------------------------------
IMAGE SMOOTHENING IN FREQUENCY DOMAIN USING DISCRETE FOURIER TRANSFORMATION AND IDEAL FILTER
IMAGE SMOOTHENING IN FREQUENCY DOMAIN USING DISCRETE FOURIER TRANSFORMATION AND GAUSSIAN FILTER
IMAGE SMOOTHENING IN FREQUENCY DOMAIN USING DISCRETE FOURIER TRANSFORMATION AND BUTTERWORTH FILTER
Frequency Domain Low pass Filter.cpp : Defines the entry point for the console application.
.................................................................................................*/

/*--------------------------------------HEADER FILES --------------------------------------------*/
// Include Necessary Header Files 
// [both in Std and Non Std Path]
#include "stdafx.h"
#include<stdio.h>
#include<math.h>
#include<time.h>
#include<opencv\cv.h>
#include<opencv\highgui.h>
using namespace std;
using namespace cv;

/*----------------------------------------- END -------------------------------------------------*/

/*------------------------------ IDEAL LOWPASS FILTER -------------------------------------------*/
// Low Pass Ideal Filter Construction Function Defination
// Input Parameter: 2D FILTER , CUTOFF FREQUENCY
void LP_Ideal_Filter(Mat &LPFilter, int CUTOFF)
{
	// Temporary Mat Variable to Store Gaussian Coefficients
	// Calculate Mid Point of Mat Variable [LPFilter]
	Mat Filter = Mat(LPFilter.rows, LPFilter.cols, CV_32F);
	Point Mid = Point(LPFilter.rows / 2, LPFilter.cols / 2);
	double D;

	// Compute Point Wise Low Pass Ideal Coefficients and Store in Filter Variable
	for (int i = 0; i < LPFilter.rows; i++)
	{
		for (int j = 0; j < LPFilter.cols; j++)
		{
			D = pow(Mid.x - abs(i - Mid.x), 2.0) + pow(Mid.y - abs(j - Mid.y), 2.0);
			Filter.at<float>(i, j) = (float)((sqrt(D) < CUTOFF) ? 1.0 : 0.0);
		}
	}

	// Construct A Filter of DFTTrans_Image[0] Size from Filter Variable
	Mat tmp[] = { Filter, Filter };
	merge(tmp, 2, LPFilter);
}

/*----------------------------------------- END --------------------------------------------------*/

/*------------------------------ GAUSSIAN LOWPASS FILTER -----------------------------------------*/
// Low Pass Gaussian Filter Construction Function Defination
// Input Parameter: 2D FILTER , CUTOFF FREQUENCY
void LP_Gaussian_Filter(Mat &LPFilter, int CUTOFF)
{
	// Temporary Mat Variable to Store Gaussian Coefficients
	// Calculate Mid Point of Mat Variable [LPFilter]
	Mat Filter = Mat(LPFilter.rows, LPFilter.cols, CV_32F);
	Point Mid = Point(LPFilter.rows / 2, LPFilter.cols / 2);
	double D;

	// Compute Point Wise Low Pass Gaussian Coefficients and Store in Filter Variable
	for (int i = 0; i < LPFilter.rows; i++)
	{
		for (int j = 0; j < LPFilter.cols; j++)
		{
			D = pow(Mid.x - abs(i - Mid.x), 2.0) + pow(Mid.y - abs(j - Mid.y), 2.0);
			Filter.at<float>(i, j) = (float)exp(-1 * D / (2 * pow(CUTOFF, 2)));
		}
	}

	// Construct A Filter of DFTTrans_Image[0] Size from Filter Variable
	Mat tmp[] = { Filter, Filter };
	merge(tmp, 2, LPFilter);
}
/*--------------------------------------- END ---------------------------------------------------*/

/*------------------------------ BUTTERWORTH LOWPASS FILTER -------------------------------------*/
// Low Pass Butterworth Filter Construction Function Defination
// Input Parameter: 2D FILTER , CUTOFF FREQUENCY, ORDER of FILTER
void LP_Butterworth_Filter(Mat &LPFilter, int CUTOFF, int ord)
{
	// Temporary Mat Variable to Store Butterworth Coefficients
	// Calculate Mid Point of Mat Variable [LPFilter]
	Mat Filter = Mat(LPFilter.rows, LPFilter.cols, CV_32F);
	Point Mid = Point(LPFilter.rows / 2, LPFilter.cols / 2);
	double D;

	// Compute Point Wise Low Pass Butterworth Coefficients and Store in Filter Variable
	for (int i = 0; i < LPFilter.rows; i++)
	{
		for (int j = 0; j < LPFilter.cols; j++)
		{
			D = pow(Mid.x - abs(i - Mid.x), 2.0) + pow(Mid.y - abs(j - Mid.y), 2.0);
			Filter.at<float>(i, j) = (float)(1.0 / (1 + pow(sqrt(D) / CUTOFF, 2 * ord)));
		}
	}

	// Construct A Filter of DFTTrans_Image[0] Size from Filter Variable
	Mat tmp[] = { Filter, Filter };
	merge(tmp, 2, LPFilter);
}

/*--------------------------------------- END ---------------------------------------------------*/

/*------------------------------------- MAIN PROGRAM --------------------------------------------*/
int main(void)
{
	// 'Select', 'ord' Variables for selection of Filter and Its Order Incase Butterwoth Filter 
	// Filter 'LPFilter' Variable to Store 2D Filter's Coefficients.
	// CUTOFF Frequency to Retain Amount of Overall Details in An Image
	Mat LPFilter; int Select, ord, CUTOFF;

	// Initialize Clock Variable to compute Time Taken in millisecs
	clock_t start, end;
	double Time_Used;

	printf("Enter The Cutoff Frequency and Order of The Filter: \n");
	scanf("%d %d", &CUTOFF, &ord);
	printf("Selection of Filter [1]Ideal [2]Gaussian [3]ButterWorth:\n");
	scanf("%d", &Select);

	// Read Image Data from Image File
	Mat Image = imread("Noise_Exp.bmp", CV_LOAD_IMAGE_COLOR);

	// Check, Whether Program Open Image File
	if (!Image.data)
	{
		printf("COULDN'T OPEN OR READ IMAGE FILE");
		return -1;
	}

	// Display The Input Image
	namedWindow("NOISY IMAGE", WINDOW_NORMAL);
	imshow("NOISY IMAGE", Image);

	// Create Variables to Store Actual and Its DFT Transfomed Values of [R, G, B] Cheannels.
	Mat RGB_Image[3], DFTTrans_Image[3];

	// Start The Timer
	start = clock();

	// Copy Image.data values to RGB_Image for Extracting All Three Frames [R, G, B] 
	split(Image, RGB_Image);

	// Convert Each Channel Pixel Values from 8U to 32F
	// Discreate Fourier Transformation of Each Channel
	for (int i = 0; i < 3; i++)
	{
		RGB_Image[i].convertTo(RGB_Image[i], CV_32F);
		dft(RGB_Image[i], DFTTrans_Image[i], DFT_COMPLEX_OUTPUT);
	}

	// Make Filter Variable Same Size of DFTTrans_Image.
	LPFilter = DFTTrans_Image[0].clone();


	// Selection of Low Pass Filter
	switch (Select)
	{
	case 1: printf("You Selected Ideal Filter \n");
		LP_Ideal_Filter(LPFilter, CUTOFF);
		break;
	case 2: printf("You Selected Gaussian Filter \n");
		LP_Gaussian_Filter(LPFilter, CUTOFF);
		break;
	case 3:	printf("You Selected Butterworth \n");
		LP_Butterworth_Filter(LPFilter, CUTOFF, ord);
		break;
	default: printf("Wrong Selection \n");
		break;
	}

	// Remove Noise Using Low Pass Filter By Applying It on Channels Separately
	// Inverse Discrete Fourier Transformation of Each Channel
	// Convert Each Chanel Pixel Values from 32F to 8U
	for (int i = 0; i < 3; i++)
	{
		Mat chan[2];
		mulSpectrums(DFTTrans_Image[i], LPFilter, DFTTrans_Image[i], 0);
		idft(DFTTrans_Image[i], DFTTrans_Image[i], DFT_SCALE);
		split(DFTTrans_Image[i], chan);
		chan[0].convertTo(RGB_Image[i], CV_8U);
	}

	// Merge All Three Channels to Construct Final Image
	merge(RGB_Image, 3, Image);

	//Stop the Timer
	end = clock();

	// Calculate Time Taken for Image Smoothing by a Low Pass Filter in millisecs.
	Time_Used = (float)(end - start) / CLOCKS_PER_SEC;
	printf("Time Taken for Image smoothing : %.3f", Time_Used);

	// Display Final Smoothed Image By Gaussian FIlter
	namedWindow("SMOOTHED IMAGE", WINDOW_AUTOSIZE);
	imshow("SMOOTHED IMAGE", Image);

	// Write 2D Array to Image Fil (.bmp)
	imwrite("Exp_bw_90.bmp", Image);

	waitKey(0);
	return 0;
}
/*----------------------------------- END ------------------------------------------------------*/
