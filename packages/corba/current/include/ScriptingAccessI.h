/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:25:58 CEST 2006  ScriptingAccessI.h 

                        ScriptingAccessI.h -  description
                           -------------------
    begin                : Mon June 26 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@fmtc.be
 
 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/
 
 
// -*- C++ -*-
//
// $Id$

// ****  Code generated by the The ACE ORB (TAO) IDL Compiler ****
// TAO and the TAO IDL Compiler have been developed by:
//       Center for Distributed Object Computing
//       Washington University
//       St. Louis, MO
//       USA
//       http://www.cs.wustl.edu/~schmidt/doc-center.html
// and
//       Distributed Object Computing Laboratory
//       University of California at Irvine
//       Irvine, CA
//       USA
//       http://doc.ece.uci.edu/
// and
//       Institute for Software Integrated Systems
//       Vanderbilt University
//       Nashville, TN
//       USA
//       http://www.isis.vanderbilt.edu/
//
// Information about TAO is available at:
//     http://www.cs.wustl.edu/~schmidt/TAO.html

// TAO_IDL - Generated from 
// be/be_codegen.cpp:844

#ifndef SCRIPTINGACCESSI_H_
#define SCRIPTINGACCESSI_H_

#include "ScriptingAccessS.h"
#include "execution/ScriptingAccess.hpp"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
#pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

class  Orocos_ScriptingAccess_i : public virtual POA_Orocos::ScriptingAccess, public virtual PortableServer::RefCountServantBase
{
    ORO_Execution::ScriptingAccess* mee;
public:
  //Constructor 
  Orocos_ScriptingAccess_i (ORO_Execution::ScriptingAccess* ee);
  
  //Destructor 
  virtual ~Orocos_ScriptingAccess_i (void);
  
  virtual
  void loadPrograms (
      const char * code,
      const char * filename
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException,
      ::Orocos::ScriptingAccess::LoadException
    ));
  
  virtual
  void unloadProgram (
      const char * name
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException,
      ::Orocos::ScriptingAccess::LoadException
    ));
  
  virtual
  ::Orocos::ScriptingAccess::ProgramNames * getPrograms (
      
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
    ));
  
  virtual
  CORBA::Boolean hasProgram (
      const char * name
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
    ));
  
  virtual
  CORBA::Long getProgramLine (
      const char * name
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
    ));
  
  virtual
  char * getProgramText (
      const char * name
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
    ));
  
  virtual
  char * getProgramStatus (
      const char * name
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
    ));
  
  virtual
  void loadStateMachines (
      const char * code,
      const char * filename
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException,
      ::Orocos::ScriptingAccess::LoadException
    ));
  
  virtual
  void unloadStateMachine (
      const char * name
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException,
      ::Orocos::ScriptingAccess::LoadException
    ));
  
  virtual
  ::Orocos::ScriptingAccess::StateMachineNames * getStateMachines (
      
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
    ));
  
  virtual
  CORBA::Boolean hasStateMachine (
      const char * name
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
    ));
  
  virtual
  char * getStateMachineText (
      const char * name
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
    ));
  
  virtual
  char * getStateMachineStatus (
      const char * name
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
    ));
  
  virtual
  CORBA::Long getStateMachineLine (
      const char * name
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
    ));
  
  virtual
  char * getCurrentState (
      const char * name
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
    ));
  
  virtual
  CORBA::Boolean execute (
      const char * code
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
    ));
};


#endif /* SCRIPTINGACCESSI_H_  */

