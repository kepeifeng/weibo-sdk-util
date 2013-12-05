#include "stdafx.h"

#include <util/fsm/MachineType.hxx>

using namespace Util;

static int globalCount = 1;

MachineType::MachineType(const string& name)
   : mNumber(++globalCount),
     mName(name)
{}

 
MachineType::~MachineType()
{}

bool
MachineType::operator==(const MachineType& rhs) const
{
   return mNumber == rhs.mNumber;
}

bool
MachineType::operator!=(const MachineType& rhs) const
{
   return mNumber != rhs.mNumber;
}

ostream&
operator<<(ostream& strm, const MachineType& machineType)
{
   strm << machineType.getName();
   return strm;
}

// Copyright 2004 Purplecomm, Inc.
