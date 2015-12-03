#
# BEGIN_COPYRIGHT
#
# Copyright (C) 2008-2015 SciDB, Inc.
# All Rights Reserved.
#
# SciDB is free software: you can redistribute it and/or modify
# it under the terms of the AFFERO GNU General Public License as published by
# the Free Software Foundation.
#
# SciDB is distributed "AS-IS" AND WITHOUT ANY WARRANTY OF ANY KIND,
# INCLUDING ANY IMPLIED WARRANTY OF MERCHANTABILITY,
# NON-INFRINGEMENT, OR FITNESS FOR A PARTICULAR PURPOSE. See
# the AFFERO GNU General Public License for the complete license terms.
#
# You should have received a copy of the AFFERO GNU General Public License
# along with SciDB.  If not, see <http://www.gnu.org/licenses/agpl-3.0.html>
#
# END_COPYRIGHT
##

#The very simple makefile for the example UDOs. This is the "standalone" version. Rename this file to "Makefile"
#in order to enable it.

#The SciDB source must be present in order to build. It is specified
#As the SCIDB_SOURCE_DIR variable.

#Example: make SCIDB_SOURCE_DIR=/home/user/workspace/scidb_trunk

SCIDB_SOURCE_DIR=$$HOME/scidbtrunk/
#The scidb-boost-devel package also needs to be installed:
BOOST_LOCATION= /opt/scidb/15.7/3rdparty/boost/include/
CFLAGS = -pedantic -W -Wextra -Wall -Wno-variadic-macros -Wno-strict-aliasing \
         -Wno-long-long -Wno-unused-parameter -fPIC -D_STDC_FORMAT_MACROS \
         -Wno-system-headers -isystem  $(OPTIMIZED) -D_STDC_LIMIT_MACROS -std=c99
         
CCFLAGS=-pedantic -W -Wextra -Wall -Wno-strict-aliasing -Wno-long-long -Wno-unused-parameter -Wno-unused-local-typedefs -Wno-variadic-macros -fPIC -D__STDC_FORMAT_MACROS -Wno-system-headers -isystem -O2 -g -DNDEBUG -ggdb3  -D__STDC_LIMIT_MACROS -std=c++0x
INC=-I. -DPROJECT_ROOT="\"$(SCIDB_SOURCE_DIR)\"" -I"$(SCIDB_SOURCE_DIR)/include" -I"$(BOOST_LOCATION)"
LIBS=-L"$(SCIDB_SOURCE_DIR)/lib" -shared -Wl,-soname,libexample_udos.so -L. -lm

all:
	@if test ! -d "$(SCIDB_SOURCE_DIR)"; then echo  "Error. Try:\n\nmake SCIDB_SOURCE_DIR=<PATH TO SCIDB TRUNK>"; exit 1; fi 
	$(CXX) $(CCFLAGS) $(INC) -o plugin.cpp.o -c plugin.cpp

	#$(CXX) $(CCFLAGS) $(INC) -o LogicalHelloInstances.cpp.o -c HelloInstances/LogicalHelloInstances.cpp
	#$(CXX) $(CCFLAGS) $(INC) -o PhysicalHelloInstances.cpp.o -c HelloInstances/PhysicalHelloInstances.cpp

	$(CXX) $(CCFLAGS) $(INC) -o LogicalInstanceStats.cpp.o -c InstanceStats/LogicalInstanceStats.cpp
	$(CXX) $(CCFLAGS) $(INC) -o PhysicalInstanceStats.cpp.o -c InstanceStats/PhysicalInstanceStats.cpp


	#Shift UDO Files
	#$(CXX) $(CCFLAGS) $(INC) -o LogicalShift.cpp.o -c Shift/LogicalShift.cpp
	#$(CXX) $(CCFLAGS) $(INC) -o PhysicalInstanceStats.cpp.o -c InstanceStats/PhysicalInstanceStats.cpp

	#Kernel Window UDO Files
	#$(CXX) $(CCFLAGS) $(INC) -o WindowArray.cpp.o -c KernelWindow/WindowArray.cpp
	#$(CXX) $(CCFLAGS) $(INC) -o LogicalKernelWindow.cpp.o -c KernelWindow/LogicalKernelWindow.cpp
	#$(CXX) $(CCFLAGS) $(INC) -o PhysicalKernelWindow.cpp.o -c KernelWindow/PhysicalKernelWindow.cpp
	
	#KNN UDO Files
	$(CC) $(CFLAGS) $(INC) -c -o rowsort.o knn/rowsort.c
	$(CC) $(CFLAGS) $(INC) -c -o merge.o knn/merge.c
	$(CXX) $(CCFLAGS) $(INC) -o Logicalknn.o -c knn/Logicalknn.cpp
	$(CXX) $(CCFLAGS) $(INC) -o Physicalknn.o -c knn/Physicalknn.cpp

	#Memcpy UDO Files
	$(CXX) $(CCFLAGS) $(INC) -o LogicalMemcpy.cpp.o -c Memcpy/LogicalMemcpy.cpp
	$(CXX) $(CCFLAGS) $(INC) -o PhysicalMemcpy.cpp.o -c Memcpy/PhysicalMemcpy.cpp
	

	$(CXX) $(CCFLAGS) $(INC) -o libexample_udos.so \
	                           plugin.cpp.o \
	                           LogicalMemcpy.cpp.o \
	                           PhysicalMemcpy.cpp.o \
	                           LogicalInstanceStats.cpp.o \
	                           PhysicalInstanceStats.cpp.o \
				   merge.o\
  				   rowsort.o\
				   Logicalknn.o\
				   Physicalknn.o\
	                           $(LIBS)
				

     #                     LogicalHelloInstances.cpp.o \
	                           PhysicalHelloInstances.cpp.o \
	                           LogicalInstanceStats.cpp.o \
	                           WindowArray.cpp.o  \
	                           LogicalKernelWindow.cpp.o \
	                           PhysicalKernelWindow.cpp.o \
	                           PhysicalInstanceStats.cpp.o 	 

clean:
	rm -f *.o *.so
