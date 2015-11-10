/*
**
* BEGIN_COPYRIGHT
*
* Copyright (C) 2008-2015 SciDB, Inc.
* All Rights Reserved.
*
* SciDB is free software: you can redistribute it and/or modify
* it under the terms of the AFFERO GNU General Public License as published by
* the Free Software Foundation.
*
* SciDB is distributed "AS-IS" AND WITHOUT ANY WARRANTY OF ANY KIND,
* INCLUDING ANY IMPLIED WARRANTY OF MERCHANTABILITY,
* NON-INFRINGEMENT, OR FITNESS FOR A PARTICULAR PURPOSE. See
* the AFFERO GNU General Public License for the complete license terms.
*
* You should have received a copy of the AFFERO GNU General Public License
* along with SciDB.  If not, see <http://www.gnu.org/licenses/agpl-3.0.html>
*
* END_COPYRIGHT
*/

/**
 * @file LogicalHelloInstances.cpp
 * A sample UDO that outputs an array containing a "Hello, World" string from every instance.
 * This is the most basic UDO designed to provide a starting example and an introduction to some SciDB internals.
 * As-is, the operator is barely useful.
 *
 * @brief The operator: hello_instances().
 *
 * @par Synopsis:
 *   <br> hello_instances()
 *
 * @par Summary:
 *   <br>
 *   Produces a result array with one cell for each running SciDB instance, and a single string "hello world"
 *   attribute.
 *   <br>
 *   To add some usefulness, the operator may be extended to return some basic CPU, disk and RAM usage information
 *   from every instance, and then used for system monitoring purporses.
 *
 * @par Input: none
 *
 * @par Output array:
 *   <br> <
 *   <br>   instance_status:string
 *   <br> >
 *   <br> [
 *   <br>   instance_no = 0:*,1 0]
 *   <br> ]
 *
 * @author apoliakov@paradigm4.com
 */

#include <query/Operator.h>
//#include "ShiftSettings.h"

using namespace std;

namespace scidb //not required
{

/**
 * The Logical Operator object for hello_instances.
 * The primary objective of this class is to
 * <br> - check to make sure all the inputs are correct and
 * <br> - infer the shape (schema) of the output array, given these inputs
 *  Reminder: all operators accept zero or more arrays and parameters, and return a single array.
 */
class LogicalShift : public LogicalOperator
{
public:
    /**
     * All LogicalOperator constructors have the same signature and list the acceptable inputs here.
     * In this case, the operator does not accept any inputs.
     * @param logicalName used internally by scidb
     * @param alias used internally by scidb
     * @see LogicalInstanceStats.cpp for an example on how to list inputs.
     */
    LogicalShift(const string& logicalName, const string& alias):
        LogicalOperator(logicalName, alias)
    {}

    /**
     * Determine the schema of the output. inferSchema is called on the coordinator instance during query planning and
     * may be called several times as the planner gets its act together. It will always be called with the same inputs
     * for the same query. This function must behave deterministically, but the shape of the output may vary based on
     * inputs and parameters.
     * @param schemas all of the schemas of the input arrays (if the operator accepts any)
     * @param query the query context
     * @return the schema of the outpt, as described above.
     */
    ArrayDesc inferSchema(vector< ArrayDesc> schemas, std::shared_ptr< Query> query)
    {
    	ArrayDesc const& inputSchema = schemas[0];
		if (inputSchema.getAttributes(true).size() != 1)
		{
			throw SYSTEM_EXCEPTION(SCIDB_SE_OPERATOR, SCIDB_LE_ILLEGAL_OPERATION)
				  << "Operator uniq only accepts an input with a single attribute";
		}

		/* -- TODO: Need to push back multiple dimensions.
		if (inputSchema.getDimensions().size() != 1 || inputSchema.getDimensions()[0].getStartMin() != 0)
		{
			throw SYSTEM_EXCEPTION(SCIDB_SE_OPERATOR, SCIDB_LE_ILLEGAL_OPERATION)
				  << "Operator uniq only accepts an input with a single dimension that starts at 0";
		}*/

		AttributeDesc const& inputAttribute = inputSchema.getAttributes()[0];
		//ShiftSettings settings (_parameters, true, query);
		Attributes outputAttributes;
		outputAttributes.push_back( AttributeDesc(0,
												  inputAttribute.getName(),
												  inputAttribute.getType(),
												  0, //no longer nullable
												  inputAttribute.getDefaultCompressionMethod())); //Note:
		/* The compression feature (beyond RLE) is rarely used but here we carry the value over from the input.
		 * It only has an effect if it is not set to NONE (default) and the array is stored later in the query.
		 */
		outputAttributes = addEmptyTagAttribute(outputAttributes);
		Dimensions outputDimensions = inputSchema.getDimensions();

		return ArrayDesc(inputSchema.getName(), outputAttributes, outputDimensions, defaultPartitioning());
    }
};

//This macro registers the operator with the system. The second argument is the SciDB user-visible operator name that is
//used to invoke it.
REGISTER_LOGICAL_OPERATOR_FACTORY(LogicalShift, "shift");

} //namespace scidb
