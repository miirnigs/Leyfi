// Genetic programming tree evaluation functions for dynamical models -- pir -- 7.7.2022

// REVISION HISTORY:

//*****************************************************************************

#include <math.h>


#include <Evolutionary/GP/GPTrees.h>
#include <Evolutionary/GP/GPTreeEvaluateDynamical.h>
#include <Yiming-Wang/eaton-rawlings-dataset/eaton-rawlings.h>
#include <Evolutionary/GP/GPlibCode.h>
//#include <Functions/TestFunctions/TestFunctions.h>
#include <CDataset/StandardiseDatasets.h>
#include <Evolutionary/GP/GPTreeEvaluate.h>
#include "pir/utilities/CreateResultsDirectory.h"
using namespace std;

//*****************************************************************************

inline static double AQ(const double a, const double b)
// Binary analytic quotient
	{
	return a / sqrt(1.0 + (b * b));
	} // AQ()

//-----------------------------------------------------------------------------
const int predictionIndex=1;
double TreeEvaluateDynamicalTraining(CDataset<double>& Dataset, const int predictionIndex, const int noStepsAhead, const int accumulatedDelay, GP_TREE pTree)
// Recursively evaluate dynamical tree during training; prediction index is the index of the point at which the prediction is being made; Time-now = this index minus no. of steps ahead;

	{
	assert((predictionIndex > 1) and (predictionIndex <= Dataset.NoStoredPatterns()));
	assert(noStepsAhead >= 1);
	assert(accumulatedDelay >= 0);

	const uint32_t uPatternVectorLength = Dataset.VectorLength();
	const enNodeType_t enNodeType = NodeType(pTree->m_suNodeTag);

	// Terminal node
	if(enNodeType == enTerminal)
		{
		const int lag = predictionIndex - accumulatedDelay - 1;
		if(lag < 1)
			{
			ERROR_HANDLER("Cannot get correct actual exogenous input values: index out of range!");
			}

		CColumnVector u(uPatternVectorLength);
		u = Dataset[lag];

		const uint32_t uPatternIndex = static_cast<uint32_t>(pTree->m_suNodeTag);
		return u[uPatternIndex];
		}

	// Autoregressive terminal node
	if(NodeType(pTree->m_suNodeTag) == enAutoregressiveTerminal)
		{
		const int lag = predictionIndex - noStepsAhead - accumulatedDelay;
		if(lag < 1)
			{
			ERROR_HANDLER("Cannot get correct actual exogenous input values! Index out of range");
			}

		return Dataset.Tag(lag);
		}

	// Constant or immutable constant node
	if((NodeType(pTree->m_suNodeTag) == enConstant) or (NodeType(pTree->m_suNodeTag) == enImmutableConstant))
		{
		const double constant = static_cast<stGPConstantNode_t*>(pTree)->m_dConstant;
		return constant;
		}

	// Interior nodes
	switch(pTree->m_suNodeTag)
		{
		case suUnaryMinusNode:
				{
				const double value = TreeEvaluateDynamicalTraining(Dataset, predictionIndex, noStepsAhead, accumulatedDelay, static_cast<stGPUnaryNode_t*>(pTree)->m_pstLeftNode);
				return -(value);
				break;
				}

		case suOneBackshiftNode:
				{
				const int newDelay = accumulatedDelay + 1;
				const double value = TreeEvaluateDynamicalTraining(Dataset, predictionIndex, noStepsAhead, newDelay, static_cast<stGPUnaryNode_t*>(pTree)->m_pstLeftNode);

				return value;
				break;
				}

		case suTwoBackshiftNode:
				{
				const int newDelay = accumulatedDelay + 2;
				const double value = TreeEvaluateDynamicalTraining(Dataset, predictionIndex, noStepsAhead, newDelay, static_cast<stGPUnaryNode_t*>(pTree)->m_pstLeftNode);

				return value;
				break;
				}

		case suFourBackshiftNode:
				{
				const int newDelay = accumulatedDelay + 4;
				const double value = TreeEvaluateDynamicalTraining(Dataset, predictionIndex, noStepsAhead, newDelay, static_cast<stGPUnaryNode_t*>(pTree)->m_pstLeftNode);

				return value;
				break;
				}

		case suBinarySubtractNode:
				{
				const double leftValue = TreeEvaluateDynamicalTraining(Dataset, predictionIndex, noStepsAhead, accumulatedDelay, static_cast<stGPBinaryNode_t*>(pTree)->m_pstLeftNode);
				const double rightValue = TreeEvaluateDynamicalTraining(Dataset, predictionIndex, noStepsAhead, accumulatedDelay, static_cast<stGPBinaryNode_t*>(pTree)->m_pstMiddleNode);

				return (leftValue - rightValue);
				break;
				}

		case suBinaryAddNode:
				{
				const double leftValue = TreeEvaluateDynamicalTraining(Dataset, predictionIndex, noStepsAhead, accumulatedDelay, static_cast<stGPBinaryNode_t*>(pTree)->m_pstLeftNode);
				const double rightValue = TreeEvaluateDynamicalTraining(Dataset, predictionIndex, noStepsAhead, accumulatedDelay, static_cast<stGPBinaryNode_t*>(pTree)->m_pstMiddleNode);

				return (leftValue + rightValue);
				break;
				}

		case suBinaryMultiplyNode:
				{
				const double leftValue = TreeEvaluateDynamicalTraining(Dataset, predictionIndex, noStepsAhead, accumulatedDelay, static_cast<stGPBinaryNode_t*>(pTree)->m_pstLeftNode);
				const double rightValue = TreeEvaluateDynamicalTraining(Dataset, predictionIndex, noStepsAhead, accumulatedDelay, static_cast<stGPBinaryNode_t*>(pTree)->m_pstMiddleNode);

				return (leftValue * rightValue);
				break;
				}

		case suBinaryAnalyticQuotient:
				{
				const double leftValue = TreeEvaluateDynamicalTraining(Dataset, predictionIndex, noStepsAhead, accumulatedDelay, static_cast<stGPBinaryNode_t*>(pTree)->m_pstLeftNode);
				const double rightValue = TreeEvaluateDynamicalTraining(Dataset, predictionIndex, noStepsAhead, accumulatedDelay, static_cast<stGPBinaryNode_t*>(pTree)->m_pstMiddleNode);

				return AQ(leftValue, rightValue);
				break;
				}

		default:
			ERROR_HANDLER("Unrecognised node type!");
		}

	return NAN;	// Return NaN -- on error condition
	} // TreeEvaluateDynamicalTraining()

//-----------------------------------------------------------------------------

double TreeEvaluateDynamicalPrediction(CRingBuffer<ringbufferItem_t>& ringbuffer, vector<double> u, const int noStepsAhead, const int accumulatedDelay, GP_TREE pTree)
// Recursively evaluate dynamical tree during MPC; assumes persistent disturbance variables


	{
	assert(noStepsAhead >= 1);
	assert(accumulatedDelay >= 0);

	const enNodeType_t enNodeType = NodeType(pTree->m_suNodeTag);

	// Terminal node
	if(enNodeType == enTerminal)
		{
		const int lag = predictionIndex - accumulatedDelay - 1;
		if(lag < 1)
			{
			ERROR_HANDLER("Cannot get correct actual exogenous input values: index out of range!");
			}

		CColumnVector u = (uPatternVectorLength);
		u = ringbuffer[lag];

		const uint32_t uPatternIndex = static_cast<uint32_t>(pTree->m_suNodeTag);
		return u[uPatternIndex];
		}

	// Autoregressive terminal node
	if(NodeType(pTree->m_suNodeTag) == enAutoregressiveTerminal)
		{
		const int lag = predictionIndex - noStepsAhead - accumulatedDelay;
		if(lag < 1)
			{
			ERROR_HANDLER("Cannot get correct actual exogenous input values! Index out of range");
			};

		return CDataset.Tag(lag);
		}

	// Constant or immutable constant node
	if((NodeType(pTree->m_suNodeTag) == enConstant) or (NodeType(pTree->m_suNodeTag) == enImmutableConstant))
		{
		const double constant = static_cast<stGPConstantNode_t*>(pTree)->m_dConstant;
		return constant;
		}

	// Interior nodes
	switch(pTree->m_suNodeTag)
		{
		case suUnaryMinusNode:
				{
				const double value = TreeEvaluateDynamicalPrediction(ringbuffer, u, noStepsAhead, accumulatedDelay, static_cast<stGPUnaryNode_t*>(pTree)->m_pstLeftNode);
				return -(value);
				break;
				}

		case suOneBackshiftNode:
				{
				const int newDelay = accumulatedDelay + 1;
				const double value = TreeEvaluateDynamicalPrediction(ringbuffer, u, noStepsAhead, newDelay, static_cast<stGPUnaryNode_t*>(pTree)->m_pstLeftNode);

				return value;
				break;
				}

		case suTwoBackshiftNode:
				{
				const int newDelay = accumulatedDelay + 2;
				const double value = TreeEvaluateDynamicalPrediction(ringbuffer, u, noStepsAhead, newDelay, static_cast<stGPUnaryNode_t*>(pTree)->m_pstLeftNode);

				return value;
				break;
				}

		case suFourBackshiftNode:
				{
				const int newDelay = accumulatedDelay + 4;
				const double value = TreeEvaluateDynamicalPrediction(ringbuffer, u, noStepsAhead, newDelay, static_cast<stGPUnaryNode_t*>(pTree)->m_pstLeftNode);

				return value;
				break;
				}

		case suBinarySubtractNode:
				{
				const double leftValue = TreeEvaluateDynamicalPrediction(ringbuffer, u, noStepsAhead, accumulatedDelay, static_cast<stGPBinaryNode_t*>(pTree)->m_pstLeftNode);
				const double rightValue = TreeEvaluateDynamicalPrediction(ringbuffer, u, noStepsAhead, accumulatedDelay, static_cast<stGPBinaryNode_t*>(pTree)->m_pstMiddleNode);

				return (leftValue - rightValue);
				break;
				}

		case suBinaryAddNode:
				{
				const double leftValue = TreeEvaluateDynamicalPrediction(ringbuffer, u, noStepsAhead, accumulatedDelay, static_cast<stGPBinaryNode_t*>(pTree)->m_pstLeftNode);
				const double rightValue = TreeEvaluateDynamicalPrediction(ringbuffer, u, noStepsAhead, accumulatedDelay, static_cast<stGPBinaryNode_t*>(pTree)->m_pstMiddleNode);

				return (leftValue + rightValue);
				break;
				}

		case suBinaryMultiplyNode:
				{
				const double leftValue = TreeEvaluateDynamicalPrediction(ringbuffer, u, noStepsAhead, accumulatedDelay, static_cast<stGPBinaryNode_t*>(pTree)->m_pstLeftNode);
				const double rightValue = TreeEvaluateDynamicalPrediction(ringbuffer, u, noStepsAhead, accumulatedDelay, static_cast<stGPBinaryNode_t*>(pTree)->m_pstMiddleNode);

				return (leftValue * rightValue);
				break;
				}

		case suBinaryAnalyticQuotient:
				{
				const double leftValue = TreeEvaluateDynamicalPrediction(ringbuffer, u, noStepsAhead, accumulatedDelay, static_cast<stGPBinaryNode_t*>(pTree)->m_pstLeftNode);
				const double rightValue = TreeEvaluateDynamicalPrediction(ringbuffer, u, noStepsAhead, accumulatedDelay, static_cast<stGPBinaryNode_t*>(pTree)->m_pstMiddleNode);

				return AQ(leftValue, rightValue);
				break;
				}

		default:
			ERROR_HANDLER("Unrecognised node type!");
		}

	return NAN;	// Return NaN -- on error condition
	} // TreeEvaluateDynamicalPrediction()

//-----------------------------------------------------------------------------

//double TreeEvaluateDynamicalAssumeConstantWF(CDataset<stOSAtag_t>& Dataset,
//		const uint32_t uPredictionIndex,
//		const uint32_t uAccumulatedDelay,
//		GP_TREE pstTree,
//		const uint32_t uDatasetRecordIndex)
//// Recursively evaluate dynamical tree; assuming the weather variables stay the same over the prediction horizon
//	{
//	const uint32_t uPatternVectorLength = Dataset.VectorLength();
//
//	// Terminal node
//	if(NodeType(pstTree->m_suNodeTag) == enTerminal)
//		{
//		const uint32_t uPatternIndex = static_cast<uint32_t>(pstTree->m_suNodeTag);
//
//		//Assume MFR is the last input
//		if(uPatternIndex == uPatternVectorLength)//If MFR values is required, return the exact MFR values.
//			{
//			const int32_t nLag = static_cast<int32_t>(uPredictionIndex) - static_cast<int32_t>(uAccumulatedDelay);
//			if(nLag < 1)
//				{
//				ERROR_HANDLER("Cannot get correct actual exogenous input values !");
//				}
//
//			CColumnVector InputVector(uPatternVectorLength);
//			InputVector = Dataset[nLag];
//			return InputVector[uPatternIndex];
//			}
//		else //If Weather Variables are required, return persistent values
//			{
//			CColumnVector InputVector(uPatternVectorLength);
//			InputVector = Dataset[uDatasetRecordIndex];
//			return InputVector[uPatternIndex];
//			}
//		}
//
//	// Predefined constant node
//	if(NodeType(pstTree->m_suNodeTag) == enConstant)
//		{
//		const double dConstant = static_cast<stGPConstantNode_t*>(pstTree)->m_dConstant;
//		return dConstant;
//		}
//
//	// Autoregressive terminal node
//	if(NodeType(pstTree->m_suNodeTag) == enAutoregressiveTerminal)
//		{
//		int32_t nLag = static_cast<int32_t>(uPredictionIndex - 1) - static_cast<int32_t>(uAccumulatedDelay);
//		if(nLag < 1)
//			{
//			ERROR_HANDLER("Cannot get correct actual autoregressive values !");
//			}
//
//		return Dataset.Tag(nLag).dPrediction;
//		}
//
//	switch(pstTree->m_suNodeTag)
//		{
//		case suUnaryMinusNode:
//				{
//				const double dValue = TreeEvaluateDynamicalAssumeConstantWF(Dataset, uPredictionIndex,
//									  uAccumulatedDelay,
//									  static_cast<stGPUnaryNode_t*>(pstTree)->m_pstLeftNode,
//									  uDatasetRecordIndex);
//
//				return (-dValue);
//				break;
//				}
//
//		case suOneBackshiftNode:
//				{
//				const uint32_t uDelay = uAccumulatedDelay + 1;
//				const double dValue = TreeEvaluateDynamicalAssumeConstantWF(Dataset, uPredictionIndex,
//									  uDelay, static_cast<stGPUnaryNode_t*>(pstTree)->m_pstLeftNode,
//									  uDatasetRecordIndex);
//
//				return dValue;
//				break;
//				}
//
//		case suTwoBackshiftNode:
//				{
//				const uint32_t uDelay = uAccumulatedDelay + 2;
//				const double dValue = TreeEvaluateDynamicalAssumeConstantWF(Dataset, uPredictionIndex, uDelay,
//									  static_cast<stGPUnaryNode_t*>(pstTree)->m_pstLeftNode,
//									  uDatasetRecordIndex);
//
//				return dValue;
//				break;
//				}
//
//		case suFourBackshiftNode:
//				{
//				const uint32_t uDelay = uAccumulatedDelay + 4;
//				const double dValue = TreeEvaluateDynamicalAssumeConstantWF(Dataset, uPredictionIndex, uDelay,
//									  static_cast<stGPUnaryNode_t*>(pstTree)->m_pstLeftNode,
//									  uDatasetRecordIndex);
//				return dValue;
//				break;
//				}
//
//		case suBinarySubtractNode:
//				{
//				const double dLeftValue = TreeEvaluateDynamicalAssumeConstantWF(Dataset, uPredictionIndex, uAccumulatedDelay,
//										  static_cast<stGPBinaryNode_t*>(pstTree)->m_pstLeftNode,
//										  uDatasetRecordIndex);
//				const double dRightValue = TreeEvaluateDynamicalAssumeConstantWF(Dataset, uPredictionIndex, uAccumulatedDelay,
//										   static_cast<stGPBinaryNode_t*>(pstTree)->m_pstMiddleNode,
//										   uDatasetRecordIndex);
//
//				return (dLeftValue - dRightValue);
//				break;
//				}
//
//		case suBinaryAddNode:
//				{
//				const double dLeftValue = TreeEvaluateDynamicalAssumeConstantWF(Dataset, uPredictionIndex, uAccumulatedDelay,
//										  static_cast<stGPBinaryNode_t*>(pstTree)->m_pstLeftNode,
//										  uDatasetRecordIndex);
//				const double dRightValue = TreeEvaluateDynamicalAssumeConstantWF(Dataset, uPredictionIndex, uAccumulatedDelay,
//										   static_cast<stGPBinaryNode_t*>(pstTree)->m_pstMiddleNode,
//										   uDatasetRecordIndex);
//
//				return (dLeftValue + dRightValue);
//				break;
//				}
//
//		case suBinaryMultiplyNode:
//				{
//				const double dLeftValue = TreeEvaluateDynamicalAssumeConstantWF(Dataset, uPredictionIndex, uAccumulatedDelay,
//										  static_cast<stGPBinaryNode_t*>(pstTree)->m_pstLeftNode,
//										  uDatasetRecordIndex);
//				const double dRightValue = TreeEvaluateDynamicalAssumeConstantWF(Dataset, uPredictionIndex, uAccumulatedDelay,
//										   static_cast<stGPBinaryNode_t*>(pstTree)->m_pstMiddleNode,
//										   uDatasetRecordIndex);
//
//				return (dLeftValue * dRightValue);
//				break;
//				}
//
//		case suBinaryAnalyticQuotient:
//				{
//				const double dLeftValue = TreeEvaluateDynamicalAssumeConstantWF(Dataset, uPredictionIndex, uAccumulatedDelay,
//										  static_cast<stGPBinaryNode_t*>(pstTree)->m_pstLeftNode,
//										  uDatasetRecordIndex);
//				const double dRightValue = TreeEvaluateDynamicalAssumeConstantWF(Dataset, uPredictionIndex, uAccumulatedDelay,
//										   static_cast<stGPBinaryNode_t*>(pstTree)->m_pstMiddleNode,
//										   uDatasetRecordIndex);
//
//				return AQ(dLeftValue, dRightValue);
//				break;
//				}
//
//		default:
//			ERROR_HANDLER("Unrecognised node type!");
//		} // switch-end
//
//	return NAN;	// Return NaN -- on error condition
//	} // TreeEvaluateDynamicalAssumeConstantWF()

//*****************************************************************************
