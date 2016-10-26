/*
 * MakeOSM.cpp
 *
 *  Created on: Apr 4, 2016
 *      Author: vagrant
 */

#include "SpeedPrediction.h"

namespace PredictivePowertrain {

SpeedPrediction::SpeedPrediction()
{
	initParams();

	// initialize weights of NN
    this->Wts = new std::vector<Eigen::MatrixXd*>(this->lastLayer + 1);
	for(int i = 0; i < this->lastLayer + 1; i++)
	{
		int L1 = this->totalLayers[i];
		int L2 = this->totalLayers[i+1];

		this->Wts->at(i) = new Eigen::MatrixXd(L2,L1+1);

		for(int j = 0; j <= L1; j++)
		{
			for(int k = 0; k < L2; k++)
			{
                this->Wts->at(i)->coeffRef(k, j) = -.01 + .02 * (double) std::rand() / RAND_MAX;
			}
		}
	}

	// initialize hidden layer outputs
    this->yHid = new std::vector<Eigen::MatrixXd*>(this->lastLayer);
	for(int i = 0; i < this->lastLayer; i++)
	{
		this->yHid->at(i) = new Eigen::MatrixXd(1, this->HN[i] + 1);
	}

	// initialize hidden layer inputs
    this->yInHid = new std::vector<Eigen::MatrixXd*>(this->lastLayer + 1);
	for(int i = 0; i < this->lastLayer+1; i++)
	{
		this->yInHid->at(i) = new Eigen::MatrixXd(1,this->totalLayers[i+1]);
	}
}

// create NN using inputted weights and activations
SpeedPrediction::SpeedPrediction(std::vector<Eigen::MatrixXd*>* Wts, std::vector<Eigen::MatrixXd*>* yHid, std::vector<Eigen::MatrixXd*>* yInHid)
{
    
    // must assert NN geometry matches geometr of input
	initParams();

	this->Wts = Wts;
	this->yHid = yHid;
	this->yInHid = yInHid;
}

void SpeedPrediction::initParams()
{
	// NN architectural params
	this->I = 100;						// input neurons
	this->O = 30;						// output neurons
	int HN[] = {80, 65, 50, 35, 20};	// hidden layer neurons

	// scaling parameters
	this->alpha = 10.0;					// learning rate
	this->maxSpeed = 200;				// max vehicle speed
	this->lb_offset = .1;				// lower bound offset
    this->dt = 2.0;

	// index vars
	this->HL = sizeof(HN)/4-1;			// last hidden layer
	this->lastLayer = this->HL+1;		// last of all layers

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
    
    // seed random gen
    std::srand(std::time(0));
}

int SpeedPrediction::getNumLayers()
{
    return this->lastLayer+2;
}

// feed-forward prediction
// assumes spd_in is historical data
void SpeedPrediction::predict(Eigen::MatrixXd * spd_in, Eigen::MatrixXd * spd_out)
{
	// fill input buffer with speed values
	Eigen::MatrixXd x = *spd_in;

	// **input to hidden layers
	for(int i = 0; i < this->HN[0]; i++)
	{
        Eigen::MatrixXd tempWts = this->Wts->at(0)->row(i);
		double act = (x*tempWts.transpose())(0);
        this->yInHid->at(0)->coeffRef(0,i) = act;
        this->yHid->at(0)->coeffRef(0,i) = 1 / (1 + std::exp(-act));
	}
    this->yHid->at(0)->coeffRef(0,this->HN[0]) = 1;

	// **through hidden layers
	for(int i = 1; i <= this->HL; i++)
	{
		for(int j = 0; j < this->HN[i]; j++)
		{
            Eigen::MatrixXd tempWts = this->Wts->at(i)->row(j);
			double act = ((*this->yHid->at(i-1))*tempWts.transpose())(0);
            this->yInHid->at(i)->coeffRef(0,j) = act;
            this->yHid->at(i)->coeffRef(0,j) = 1 / (1 + std::exp(-act));
		}
        this->yHid->at(i)->coeffRef(0,this->HN[i]) = 1;
	}

	// **hidden to output layers
	for(int i = 0; i < this->O; i++)
	{
        Eigen::MatrixXd tempWts = this->Wts->at(this->lastLayer)->row(i);
		double act = ((*this->yHid->at(this->lastLayer-1))*tempWts.transpose())(0);
        this->yInHid->at(this->lastLayer)->coeffRef(0,i) = act;
        (*spd_out)(0,i) = 1 / (1 + std::exp(-act));
	}
}

// back-propagation
void SpeedPrediction::train(Eigen::MatrixXd * spd_pred, Eigen::MatrixXd  * spd_act, Eigen::MatrixXd * spd_in)
{
	// delta matrix back propagated though all layers
	Eigen::MatrixXd abc = Eigen::MatrixXd::Zero(this->O,this->totalLayers[this->lastLayer]+1);
	Eigen::MatrixXd x = *spd_in;

	// **propagate output to last hidden layer
	for(int j = 0; j < this->O; j++)
	{
		double outError = (*spd_act)(0,j) - (*spd_pred)(0,j);
		double outDelta = outError*(*spd_pred)(0,j)*(1-(*spd_pred)(0,j));
        Eigen::MatrixXd tempWts = this->Wts->at(this->lastLayer)->row(j);

		abc.row(j) = tempWts*outDelta;
		this->Wts->at(this->lastLayer)->row(j) = tempWts+this->alpha*(*this->yHid->at(this->lastLayer-1))*outDelta;
	}

	// **propagate output through all hidden layers
	int index = 0;
	int index2 = 1;
	for(int i = this->lastLayer-1; i > 0; i--)
	{
		// create some dimension dependent locals
        Eigen::MatrixXd yHidTemp = (*this->yHid->at(i));
		Eigen::MatrixXd hidDelta = abc.transpose();
		Eigen::MatrixXd ones = Eigen::MatrixXd::Ones(1,yHidTemp.cols());

		// compute the hidden layer delta for each neuron
		for(int j = 0; j < abc.rows(); j++)
		{
			hidDelta.col(j) = abc.row(j).transpose()*(ones-yHidTemp)*yHidTemp.transpose();
		}

		// update weights on the hidden layer neurons and include input bias
        Eigen::MatrixXd yHtemp = (*this->yHid->at(i-1));
        Eigen::MatrixXd tempWts = (*this->Wts->at(i));

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
			this->Wts->at(i)->row(j) = tempWts.col(j);
		}

		index++;
		index2++;
	}

	// **propagate from hidden to input layer
    Eigen::MatrixXd yHidTemp = (*this->yHid->at(this->HL-index));
	Eigen::MatrixXd hidDelta = abc.transpose();
	Eigen::MatrixXd ones = Eigen::MatrixXd::Ones(1,yHidTemp.cols());

	// compute hidden deltas
	for(int j = 0; j < abc.rows(); j++)
	{
		hidDelta.col(j) = abc.row(j).transpose()*(ones-yHidTemp)*yHidTemp.transpose();
	}

	// update weights on the hidden layer neurons
    Eigen::MatrixXd tempWts = (*this->Wts->at(0));
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
		this->Wts->at(0)->row(j) = tempWts.col(j);
	}
}

// send address of weights
std::vector<std::vector<Eigen::MatrixXd*>*>* SpeedPrediction::getVals()
{
    std::vector<std::vector<Eigen::MatrixXd*>*>* returnList = new std::vector<std::vector<Eigen::MatrixXd*>*>(3);
	returnList->at(0) = this->Wts;
	returnList->at(1) = this->yHid;
	returnList->at(2) = this->yInHid;
	return returnList;
}

int SpeedPrediction::getI()
{
	return this->I;
}

int SpeedPrediction::getO()
{
	return this->O;
}

// scale input data and concatenate bias term
void SpeedPrediction::formatInData(Eigen::MatrixXd * input)
{
	assert((*input).cols() == this->I+1);
	Eigen::MatrixXd offset = Eigen::MatrixXd::Ones(1,this->I+1) * this->lb_offset;
	(*input) = (*input) / this->maxSpeed + offset;
	(*input)(0,this->I) = 1;
}
    
void SpeedPrediction::scaleTrainingSpeed(Eigen::MatrixXd * input)
{
    // only accept row vector
    assert((*input).rows() == 1);
    Eigen::MatrixXd offset = Eigen::MatrixXd::Ones(1,(*input).cols()) * this->lb_offset;
    (*input) = (*input) / this->maxSpeed + offset;
}
    
void SpeedPrediction::unscaleTrainingSpeed(Eigen::MatrixXd * output)
{
    // accept only row vector
    assert((*output).rows() == 1);
    Eigen::MatrixXd offset = Eigen::MatrixXd::Ones(1,(*output).cols()) * this->lb_offset;
    (*output) = (*output) * this->maxSpeed - offset;
}

// scale output data
void SpeedPrediction::formatOutData(Eigen::MatrixXd * output)
{
	assert((*output).cols() == this->O);
	Eigen::MatrixXd offset = Eigen::MatrixXd::Ones(1,this->O) * this->lb_offset;
	(*output) = (*output) * this->maxSpeed - offset;
}

void SpeedPrediction::printAll()
{
	// Weights
	std::cout << "Wts" << std::endl;
	for(int i = 0; i < this->lastLayer + 1; i++)
	{
		std::cout << (*this->Wts->at(i)) << std::endl << std::endl;
	}

	// yHid
	std::cout << "yHid" << std::endl;
	for(int i = 0; i < this->lastLayer; i++)
	{
		std::cout << (*this->yHid->at(i)) << std::endl << std::endl;
	}

	// yInHid
	std::cout << "yInHid" << std::endl;
	for(int i = 0; i < this->lastLayer+1; i++)
	{
		std::cout << (*this->yInHid->at(i)) << std::endl << std::endl;
	}

	// last layer
	std::cout << "lastLayer" << std::endl;
	std::cout << this->lastLayer << std::endl << std::endl;

	// last hidden layer
	std::cout << "last hidden layer" << std::endl;
	std::cout << this->HL << std::endl << std::endl;
}

void SpeedPrediction::setVals(std::vector<std::vector<Eigen::MatrixXd*>*>* vals)
{
    this->Wts = vals->at(0);
    this->yHid = vals->at(1);
    this->yInHid = vals->at(2);
}

float SpeedPrediction::getDT()
{
    return this->dt;
}

}

