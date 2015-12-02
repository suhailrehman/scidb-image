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
 * @file PhysicalHelloInstances.cpp
 * The physical implementation of the hello_instances operator.
 * @see LogicalHelloInstances.cpp
 * @author apoliakov@paradigm4.com
 */

#include <query/Operator.h>

using namespace std;

namespace scidb
{

/*
 * A static-linkage logger object we can use to write data to scidb.log.
 * Lookup the log4cxx package for more information.
 */
static log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("scidb.toy_operators.memcpy"));

/**
 * The primary responsibility of the PhysicalOperator is to return the proper array output as the result of the
 * execute() function.
 */
class PhysicalMemcpy : public PhysicalOperator
{
public:
    /**
     * Looks the same for all operators. All the arguments are for SciDB internal use.
     * The operator is first constucted on the coordinator during planning (possibly several times), then constructed
     * on every instance to execute. Setting internal state as a result of construction is not reliable. To avoid
     * shared-pointer cycles and potential errors, it is recommended that operators do not have any additional member
     * variables.
     */
    PhysicalMemcpy(string const& logicalName,
                           string const& physicalName,
                           Parameters const& parameters,
                           ArrayDesc const& schema):
        PhysicalOperator(logicalName, physicalName, parameters, schema)
    {}

private :


    	/**
        * Read the input array (whatever part of it we have on the local instance) and log the input data to the default logger
        */
       void printInputArrayInfo(std::shared_ptr<Array>& inputArray, InstanceID id)
       {
           std::shared_ptr<ConstArrayIterator> inputArrayIter = inputArray->getConstIterator(0);
           ostringstream out;

    	   while(!inputArrayIter->end())
           {
               ConstChunk const& inputChunk = inputArrayIter->getChunk();
               Coordinate chunkCoord = inputArrayIter->getPosition()[0]; ///get the position of the chunk

        	   out<<"Instance "<<id<<", Chunk: "<<chunkCoord<<"\n";


               std::shared_ptr<ConstChunkIterator> inputChunkIter = inputChunk.getConstIterator();

               while(!inputChunkIter->end())
               {
                   Value const& val = inputChunkIter->getItem();
            	   out<<"Instance "<<id<<", Chunk: "<<chunkCoord<<" Position: "<<inputChunkIter.get()->getPosition()<<" Value: "<<val.getDouble()<<"\n";
            	   //Dump Output
            	   ++(*inputChunkIter);
               }

               ++(*inputArrayIter);
           }

		   LOG4CXX_DEBUG(logger, out.str());

       }



    /**
     * Given the input array and an OutputArrayInfo, populate and return the local portion of the output array.
	 */

    std::shared_ptr<Array> writeOutputArray(std::shared_ptr<Array>& inputArray, std::shared_ptr<Query>& query)
    {

        std::shared_ptr<Array> outputArray(new MemArray(_schema, query));
        std::shared_ptr<ArrayIterator> outputArrayIter = outputArray->getIterator(0);

       	ostringstream out;

        std::shared_ptr<ConstArrayIterator> inputArrayIter = inputArray->getConstIterator(0);

		while(!inputArrayIter->end())
		{
			ConstChunk const& inputChunk = inputArrayIter->getChunk();
			Coordinates const& chunkCoord = inputArrayIter->getPosition(); ///get the position of the chunk

			std::shared_ptr<ConstChunkIterator> inputChunkIter = inputChunk.getConstIterator();
			std::shared_ptr<ChunkIterator> outputChunkIter = outputArrayIter->newChunk(chunkCoord).getIterator(query,
                    ChunkIterator::SEQUENTIAL_WRITE);

			out.clear();
			out<<", Chunk: "<<chunkCoord<<"\n";
			LOG4CXX_TRACE(logger, out.str());


			//TODO: Access checks on output chunk iterator:
			while(!inputChunkIter->end())
			{
				Value const& val = inputChunkIter->getItem();

				out.clear();
				out<<"Chunk: "<<chunkCoord<<" Position: "<<inputChunkIter.get()->getPosition()<<" Value: "<<val.getDouble()<<"\n";
				LOG4CXX_TRACE(logger, out.str());

	            outputChunkIter->setPosition(inputChunkIter.get()->getPosition());
	            outputChunkIter->writeItem(val);

	            ++(*inputChunkIter);
			}

			outputChunkIter->flush();
			++(*inputArrayIter);
		}

		return outputArray;
    }


    /**
     * Execute the operator and return the output array. The input arrays (with actual data) are provided as an
     * argument. Non-array arguments to the operator are set in the _parameters member variable. This particular
     * operator has no arguments. The result of the Logical***::inferSchema() method is also provided as the member
     * variable _schema. Execute is called once on each instance.
     * @param inputArrays the input array arguments. In this simple case, there are none.
     * @param query the query context
     * @return the output array object
     */
    std::shared_ptr< Array> execute(vector< std::shared_ptr< Array> >& inputArrays, std::shared_ptr<Query> query)
    {
        /* Find my instanceId from the query. Query has many useful methods like
         * - the total number of instances
         * - the id of the coordinator
         * - check if the query was cancelled.. and so on
         */
        InstanceID instanceId = query->getInstanceID();


        std::shared_ptr<Array> outputArray(new MemArray(_schema, query));

        //Print Out Array Info
        printInputArrayInfo(inputArrays[0],instanceId);
        outputArray = writeOutputArray(inputArrays[0],query);

        return outputArray;

    }
};

/* In this registration, the second argument must match the AFL operator name and the name provided in the Logical..
 * file. The third argument is arbitrary and used for debugging purposes.
 */
REGISTER_PHYSICAL_OPERATOR_FACTORY(PhysicalMemcpy, "memcpy", "PhysicalMemcpy");

} //namespace scidb
