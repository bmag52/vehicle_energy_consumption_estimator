#include <Eigen/Dense>
#include <iostream>

using Eigen::MatrixXd;

#include "speed_prediction.h"
#include <assert.h>

namespace speed_prediction {

speed_prediction::speed_prediction()
{
	init_params();

	// initialize weights of NN
	this->Wts = new Eigen::MatrixXd[this->lastLayer + 1];
	for(int i = 0; i < this->lastLayer + 1; i++)
	{
		int L1 = this->totalLayers[i];
		int L2 = this->totalLayers[i+1];

		this->Wts[i] = Eigen::MatrixXd::Zero(L2,L1+1);

		for(int j = 0; j <= L1; j++)
		{
			for(int k = 0; k < L2; k++)
			{
				this->Wts[i](k,j) = -.01 + .02 * (rand() % 10) / 10;
			}
		}
	}

	// initialize hidden layer outputs
	this->yHid = new Eigen::MatrixXd[this->lastLayer];
	for(int i = 0; i < this->lastLayer; i++)
	{
		this->yHid[i] = Eigen::MatrixXd::Zero(1,this->HN[i]+1);
	}

	// initialize hidden layer inputs
	this->yInHid = new Eigen::MatrixXd[this->lastLayer+1];
	for(int i = 0; i < this->lastLayer+1; i++)
	{
		this->yInHid[i] = Eigen::MatrixXd::Zero(1,this->totalLayers[i+1]);
	}
}

speed_prediction::speed_prediction(Eigen::MatrixXd * Wts, Eigen::MatrixXd * yHid, Eigen::MatrixXd * yInHid)
{
	init_params();

	this->Wts = Wts;
	this->yHid = yHid;
	this->yInHid = yInHid;
}

void speed_prediction::init_params()
{
	// NN architectural params
	this->alpha = 10.0;					// learning rate
	this->I = 100;						// input neurons
	this->O = 14;						// output neurons
	int HN[] = {80, 65, 50, 35, 20};	// hidden layer neurons

	// scaling parameters
	this->maxSpeed = 200;				// max vehicle speed
	this->lb_offset = .1;				// lower bound offset

	// index vars
	this->HL = sizeof(HN)/4-1;
	this->lastLayer = this->HL+1;

	// init hidden layers
	this->HN = new int[this->HL+1];
	for(int i = 0; i < this->HL+1; i++)
	{
		this->HN[i] = HN[i];
	}

	// init total layers
	this->totalLayers = new int[this->HL+3];
	this->totalLayers[0] = I;
	for(int i = 1; i <= this->HL+1; i++)
	{
		this->totalLayers[i] = this->HN[i-1];
	}
	this->totalLayers[this->lastLayer+1] = this->O;
}

// feed-forward prediction
// assumes spd_in is historical data
void speed_prediction::predict(Eigen::MatrixXd * spd_in, Eigen::MatrixXd * spd_out)
{
	// fill input buffer with speed values
	Eigen::MatrixXd x = *spd_in;

	// **input to hidden layers
	for(int i = 0; i < this->HN[0]; i++)
	{
		Eigen::MatrixXd tempWts = this->Wts[0].row(i);
		double act = (x*tempWts.transpose())(0);
		this->yInHid[0](0,i) = act;
		this->yHid[0](0,i) = 1 / (1 + exp(-act));
	}
	this->yHid[0](0,this->HN[0]) = 1;

	// **through hidden layers
	for(int i = 1; i <= this->HL; i++)
	{
		for(int j = 0; j < this->HN[i]; j++)
		{
			Eigen::MatrixXd tempWts = this->Wts[i].row(j);

			double act = (this->yHid[i-1]*tempWts.transpose())(0);
			this->yInHid[i](0,j) = act;
			this->yHid[i](0,j) = 1 / (1 + exp(-act));
		}
		this->yHid[i](0,this->HN[i]) = 1;
	}

	// **hidden to output layers
	for(int i = 0; i < this->O; i++)
	{
		Eigen::MatrixXd tempWts = this->Wts[this->lastLayer].row(i);
		double act = (this->yHid[this->lastLayer-1]*tempWts.transpose())(0);
		yInHid[this->lastLayer](0,i) = act;
		(*spd_out)(0,i) = 1 / (1 + exp(-act));
	}
}

// back-propagation
void speed_prediction::train(Eigen::MatrixXd * spd_pred, Eigen::MatrixXd  * spd_act, Eigen::MatrixXd * spd_in)
{
	// delta matrix back propagated though all layers
	Eigen::MatrixXd abc = Eigen::MatrixXd::Zero(this->O,this->totalLayers[this->lastLayer]+1);
	Eigen::MatrixXd x = *spd_in;

	// **propagate output to last hidden layer
	for(int j = 0; j < this->O; j++)
	{
		double outError = (*spd_act)(0,j) - (*spd_pred)(0,j);
		double outDelta = outError*(*spd_pred)(0,j)*(1-(*spd_pred)(0,j));
		Eigen::MatrixXd tempWts = this->Wts[this->lastLayer].row(j);

		abc.row(j) = tempWts*outDelta;
		this->Wts[this->lastLayer].row(j) = tempWts+this->alpha*this->yHid[this->lastLayer-1]*outDelta;
	}

	// **propagate output through all hidden layers
	int index = 0;
	int index2 = 1;
	for(int i = this->lastLayer-1; i > 0; i--)
	{
		// create some dimension dependent locals
		Eigen::MatrixXd yHidTemp = this->yHid[i];
		Eigen::MatrixXd hidDelta = abc.transpose();
		Eigen::MatrixXd ones = Eigen::MatrixXd::Ones(1,yHidTemp.cols());

		// compute the hidden layer delta for each neuron
		for(int j = 0; j < abc.rows(); j++)
		{
			hidDelta.col(j) = abc.row(j).transpose()*(ones-yHidTemp)*yHidTemp.transpose();
		}

		// update weights on the hidden layer neurons and include input bias
		Eigen::MatrixXd yHtemp = this->yHid[i-1];
		Eigen::MatrixXd tempWts = this->Wts[i];

		// before updating weights, calculate the delta abc without bias term
		abc = Eigen::MatrixXd::Zero(this->O,this->totalLayers[i]+1);
		for(int j = 0; j < hidDelta.cols(); j++)
		{
			Eigen::MatrixXd tempHidDelta = hidDelta.col(j).head(this->HN[this->HL-index]);
			Eigen::MatrixXd abc_col = tempWts.transpose()*tempHidDelta;
			abc.row(j) = abc_col.transpose();
		}

		// continue with updation of weights
		Eigen::MatrixXd intermediateCalc = Eigen::MatrixXd::Zero(this->HN[this->HL-index2]+1,this->HN[this->HL-index]+1);
		for(int j = 0; j < hidDelta.cols(); j++)
		{
			intermediateCalc = intermediateCalc + this->alpha*yHtemp.transpose()*hidDelta.col(j).transpose();
		}

		Eigen::MatrixXd tempWts_T = tempWts.transpose();
		tempWts = tempWts_T + intermediateCalc.topLeftCorner(intermediateCalc.rows(),intermediateCalc.cols()-1);

		for(int j = 0; j < this->HN[this->HL-index]; j++)
		{
			this->Wts[i].row(j) = tempWts.col(j);
		}

		index++;
		index2++;
	}

	// **propagate from hidden to input layer
	Eigen::MatrixXd yHidTemp = this->yHid[this->HL-index];
	Eigen::MatrixXd hidDelta = abc.transpose();
	Eigen::MatrixXd ones = Eigen::MatrixXd::Ones(1,yHidTemp.cols());

	// compute hidden deltas
	for(int j = 0; j < abc.rows(); j++)
	{
		hidDelta.col(j) = abc.row(j).transpose()*(ones-yHidTemp)*yHidTemp.transpose();
	}

	// update weights on the hidden layer neurons
	Eigen::MatrixXd tempWts = this->Wts[0];
	Eigen::MatrixXd yHtemp = Eigen::MatrixXd::Zero(1,this->I+1);
	yHtemp = x;
	yHtemp(0,this->I) = 1;

	// continue with the updation of weights
	Eigen::MatrixXd intermediateCalc = Eigen::MatrixXd::Zero(this->I+1,this->HN[0]+1);
	for(int j = 0; j < hidDelta.cols(); j++)
	{
		intermediateCalc = intermediateCalc + this->alpha*yHtemp.transpose()*hidDelta.col(j).transpose();
	}

	Eigen::MatrixXd tempWts_T = tempWts.transpose();
	tempWts = tempWts_T + intermediateCalc.topLeftCorner(intermediateCalc.rows(),intermediateCalc.cols()-1);

	for(int j = 0; j < this->HN[0]; j++)
	{
		this->Wts[0].row(j) = tempWts.col(j);
	}
}

// send address of weights
// TODO have a copy function
void speed_prediction::get_vals(Eigen::MatrixXd * Wts, Eigen::MatrixXd * yHid, Eigen::MatrixXd * yInHid)
{
	Wts = this->Wts;
	yHid = this->yHid;
	yInHid = this->yInHid;
}

int speed_prediction::get_I()
{
	return this->I;
}

int speed_prediction::get_O()
{
	return this->O;
}

// scale input data and concatenate bias term
void speed_prediction::format_inData(Eigen::MatrixXd * input)
{
	assert((*input).cols() == this->I+1);
	Eigen::MatrixXd offset = Eigen::MatrixXd::Ones(1,this->I+1) * this->lb_offset;
	(*input) = (*input) / this->maxSpeed + offset;
	(*input)(0,this->I) = 1;
}

// scale output data
void speed_prediction::format_outData(Eigen::MatrixXd * output)
{
	assert((*output).cols() == this->O);
	Eigen::MatrixXd offset = Eigen::MatrixXd::Ones(1,this->O) * this->lb_offset;
	(*output) = (*output) * this->maxSpeed - offset;
}

void speed_prediction::print_all()
{
	// Weights
	std::cout << "Wts" << std::endl;
	for(int i = 0; i < this->lastLayer + 1; i++)
	{
		std::cout << this->Wts[i] << std::endl << std::endl;
	}

	// yHid
	std::cout << "yHid" << std::endl;
	for(int i = 0; i < this->lastLayer; i++)
	{
		std::cout << this->yHid[i] << std::endl << std::endl;
	}

	// yInHid
	std::cout << "yInHid" << std::endl;
	for(int i = 0; i < this->lastLayer+1; i++)
	{
		std::cout << this->yInHid[i] << std::endl << std::endl;
	}

	// last layer
	std::cout << "lastLayer" << std::endl;
	std::cout << this->lastLayer << std::endl << std::endl;

	// last hidden layer
	std::cout << "last hidden layer" << std::endl;
	std::cout << this->HL << std::endl << std::endl;
}

}

