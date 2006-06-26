/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:25:58 CEST 2006  ExecutionI.h 

                        ExecutionI.h -  description
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
// be/be_codegen.cpp:859

// must be outside of #ifdef macro.
#include "corelib/DataSource.hpp"

#ifndef INCLUDE_EXECUTIONI_H_
#define INCLUDE_EXECUTIONI_H_

#include "ExecutionS.h"
#include "CorbaConversion.hpp"
#include <corelib/CommandInterface.hpp>
#include <corelib/Logger.hpp>

namespace ORO_Execution
{
    class CommandC;
}

#if !defined (ACE_LACKS_PRAGMA_ONCE)
#pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

template< class DataType>
class  Orocos_Expression_i
  : public virtual POA_Orocos::Expression
{
protected:
    typename ORO_CoreLib::DataSource<DataType>::const_ptr morig;
    typename ORO_CoreLib::DataSource<DataType>::value_t last_value;
public:
    typedef DataType SourceType;
    typedef typename ORO_CoreLib::DataSource<DataType>::value_t ResultType;

  // Constructor 
  Orocos_Expression_i (typename ORO_CoreLib::DataSource<SourceType>::const_ptr orig)
      : morig( orig ), last_value()
    {}
  
  // Destructor 
    virtual ~Orocos_Expression_i (void) {}
  
  virtual
  CORBA::Any* value (
      
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
      )) {
      return ORO_CoreLib::AnyConversion<ResultType>::createAny( last_value );
  }
  
  virtual
  CORBA::Boolean evaluate (
      
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
      )) {
      last_value = morig->get();
      return 1;
  }
  
  virtual
  CORBA::Any * get (
      
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
      )) {
      return ORO_CoreLib::AnyConversion<ResultType>::createAny( morig->get() );
  }
  
  virtual
  char * getType (
      
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
      )) {
      return CORBA::string_dup( morig->getType().c_str() );
  }

  virtual
  char * getTypeName (
      
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
      )) {
      return CORBA::string_dup( morig->getTypeName().c_str() );
  }
};

/**
 * A servant which serves a DataSource through the 'Any' methods.
 * This servant is inferior to the template based ones.
 */
class  Orocos_AnyExpression_i
  : public virtual POA_Orocos::Expression
{
protected:
    ORO_CoreLib::DataSourceBase::const_ptr morig;
    CORBA::Any_var last_value;
public:
    typedef ORO_CoreLib::DataSourceBase::const_ptr SourceType;
    typedef CORBA::Any                             ResultType;

    virtual void copy( ORO_CoreLib::DataSourceBase::shared_ptr new_ds ) {
        morig = new_ds;
    }

  // Constructor 
  Orocos_AnyExpression_i (ORO_CoreLib::DataSourceBase::const_ptr orig)
      : morig( orig ), last_value( morig->createAny() ) // create default Any.
    {}
  
  // Destructor 
    virtual ~Orocos_AnyExpression_i (void) {}
  
  virtual
  CORBA::Any* value (
      
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
      )) {
      return new CORBA::Any( last_value.in() );
  }
  
  virtual
  CORBA::Boolean evaluate (
      
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
      )) {
      last_value = morig->getAny();
      bool result = true;
      // if it is a bool, update result and return it, otherwise, just return true:
      ORO_CoreLib::AnyConversion<bool>::update( last_value.in(), result );
      return result;
  }
  
  virtual
  CORBA::Any * get (
      
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
      )) {
      return morig->getAny();
  }
  
  virtual
  char * getType (
      
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
      )) {
      return CORBA::string_dup( morig->getType().c_str() );
  }

  virtual
  char * getTypeName (
      
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
      )) {
      return CORBA::string_dup( morig->getTypeName().c_str() );
  }
};

template<>
class  Orocos_Expression_i<void>
  : public virtual POA_Orocos::Expression
{
protected:
    ORO_CoreLib::DataSource<void>::const_ptr morig;
public:
    typedef void SourceType;
    typedef void ResultType;

  // Constructor 
  Orocos_Expression_i (ORO_CoreLib::DataSource<void>::const_ptr orig)
      : morig( orig )
    {}
  
  // Destructor 
    virtual ~Orocos_Expression_i (void) {}
  
  virtual
  CORBA::Any* value (
      
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
      )) {
      return new CORBA::Any();
  }
  
  virtual
  CORBA::Boolean evaluate (
      
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
      )) {
      morig->get();
      return 1;
  }
  
  virtual
  CORBA::Any * get (
      
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
      )) {
      morig->get();
      return new CORBA::Any();
  }
  
  virtual
  char * getType (
      
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
      )) {
      return CORBA::string_dup( morig->getType().c_str() );
  }

  virtual
  char * getTypeName (
      
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
      )) {
      return CORBA::string_dup( morig->getTypeName().c_str() );
  }
};

template< class DataType>
class  Orocos_AssignableExpression_i
  : public virtual POA_Orocos::AssignableExpression
{
    typename ORO_CoreLib::AssignableDataSource<DataType>::shared_ptr massign;
    typename ORO_CoreLib::DataSource<DataType>::value_t last_value;
public:
    typedef DataType SourceType;
    typedef typename ORO_CoreLib::DataSource<DataType>::value_t ResultType;

  // Constructor 
  Orocos_AssignableExpression_i (typename ORO_CoreLib::AssignableDataSource<SourceType>::shared_ptr assign )
      : massign( assign ), last_value() {}
  
  // Destructor 
    virtual ~Orocos_AssignableExpression_i (void) {}
  
  virtual
  CORBA::Boolean set (
      const ::CORBA::Any & value
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
      )) {
      using namespace ORO_CoreLib;
      using namespace ORO_Execution;
      if ( !ORO_CoreLib::AnyConversion<ResultType>::update( value, last_value ) ) {
          ORO_CoreLib::Logger::log() << ORO_CoreLib::Logger::Error << "Orocos::AssignableExpression: Could not assign Any to "<<massign->getType() <<"." <<ORO_CoreLib::Logger::endl
                                     <<" Tried to assign as "<< DataSource<ResultType>::GetType() << " to native type "<< DataSource<SourceType>::GetType()<< ORO_CoreLib::Logger::endl;
          return false;
      }
      massign->set(last_value);
      return true;
  }
      
  virtual
  CORBA::Any* value (
      
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
      )) {
      return ORO_CoreLib::AnyConversion<ResultType>::createAny( last_value );
  }
  
  virtual
  CORBA::Boolean evaluate (
      
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
      )) {
      last_value = massign->get();
      return 1;
  }
  
  virtual
  CORBA::Any * get (
      
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
      )) {
      return ORO_CoreLib::AnyConversion<ResultType>::createAny( massign->get() );
  }
  
  virtual
  char * getType (
      
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
      )) {
      return CORBA::string_dup( massign->getType().c_str() );
  }

  virtual
  char * getTypeName (
      
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
      )) {
      return CORBA::string_dup( massign->getTypeName().c_str() );
  }
};

class  Orocos_AnyAssignableExpression_i
    : public Orocos_AnyExpression_i,
      public virtual POA_Orocos::AssignableExpression
{
    ORO_CoreLib::DataSourceBase::shared_ptr mset;
public:

    virtual void copy( ORO_CoreLib::DataSourceBase::shared_ptr new_ds ) {
        mset = new_ds;
        morig = new_ds;
    }

  // Constructor 
  Orocos_AnyAssignableExpression_i (ORO_CoreLib::DataSourceBase::shared_ptr orig)
      : Orocos_AnyExpression_i(orig), mset( orig )
    {}
  
  // Destructor 
    virtual ~Orocos_AnyAssignableExpression_i (void) {}

  virtual
  CORBA::Boolean set (
      const ::CORBA::Any & value
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
      )) {
      return mset->update( value );
  }
      
};


// Boolean Specialisations, like in DataSource<bool>:

template<>
class  Orocos_Expression_i<bool>
  : public virtual POA_Orocos::Expression
{
protected:
    ORO_CoreLib::DataSource<bool>::const_ptr morig;
    ORO_CoreLib::DataSource<bool>::value_t last_value;
public:
    typedef bool SourceType;
    typedef bool ResultType;

  // Constructor 
  Orocos_Expression_i (ORO_CoreLib::DataSource<bool>::const_ptr orig)
      : morig( orig ), last_value()
    {}
  
  // Destructor 
    virtual ~Orocos_Expression_i (void) {}
  
  virtual
  CORBA::Any* value (
      
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
      )) {
      return ORO_CoreLib::AnyConversion<bool>::createAny( last_value );
  }
  
  virtual
  CORBA::Boolean evaluate (
      
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
      )) {
      last_value = morig->get();
      return last_value;
  }
  
  virtual
  CORBA::Any * get (
      
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
      )) {
      last_value = morig->get();
      return ORO_CoreLib::AnyConversion<bool>::createAny( last_value );
  }
  
  virtual
  char * getType (
      
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
      )) {
      return CORBA::string_dup( morig->getType().c_str() );
  }

  virtual
  char * getTypeName (
      
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
      )) {
      return CORBA::string_dup( morig->getTypeName().c_str() );
  }
};

template<>
class  Orocos_AssignableExpression_i<bool>
  : public virtual POA_Orocos::AssignableExpression
{
    ORO_CoreLib::AssignableDataSource<bool>::shared_ptr massign;
    bool last_value;
public:
    typedef bool SourceType;
    typedef bool ResultType;

  // Constructor 
  Orocos_AssignableExpression_i (ORO_CoreLib::AssignableDataSource<bool>::shared_ptr assign )
      : massign( assign ), last_value()
    {}
  
  // Destructor 
    virtual ~Orocos_AssignableExpression_i (void) {}
  
  virtual
  CORBA::Boolean set (
      const ::CORBA::Any & value
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
      )) {
      return ORO_CoreLib::AnyConversion<bool>::update( value, massign->set() );
  }
      
  virtual
  CORBA::Any* value (
      
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
      )) {
      return ORO_CoreLib::AnyConversion<bool>::createAny( last_value );
  }
  
  virtual
  CORBA::Boolean evaluate (
      
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
      )) {
      last_value = massign->evaluate();
      return last_value;
  }
  
  virtual
  CORBA::Any * get (
      
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
      )) {
      last_value =massign->get();
      return ORO_CoreLib::AnyConversion<bool>::createAny( last_value );
  }
  
  virtual
  char * getType (
      
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
      )) {
      return CORBA::string_dup( massign->getType().c_str() );
  }

  virtual
  char * getTypeName (
      
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
      )) {
      return CORBA::string_dup( massign->getTypeName().c_str() );
  }

};

class  Orocos_Action_i : public virtual POA_Orocos::Action, public virtual PortableServer::RefCountServantBase
{
    ORO_CoreLib::CommandInterface* mcom;
public:
  //Constructor 
  Orocos_Action_i ( ORO_CoreLib::CommandInterface* com );
  
  //Destructor 
    virtual ~Orocos_Action_i (void);
  
  virtual
  CORBA::Boolean execute (
      
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
      ));

  virtual
  void reset (
      
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
      ));
};

template<class T>
class  Orocos_Method_i
    : public virtual Orocos_Expression_i<T>,
      public virtual POA_Orocos::Method, public virtual PortableServer::RefCountServantBase
{
public:
    typedef T SourceType;
    typedef typename ORO_CoreLib::DataSource<T>::value_t ResultType;
    typename ORO_CoreLib::DataSource<SourceType>::shared_ptr mmethod;
  //Constructor 
  Orocos_Method_i ( typename ORO_CoreLib::DataSource<SourceType>::shared_ptr datas )
      : Orocos_Expression_i<SourceType>( datas ), mmethod( datas )
    {}
  
  //Destructor 
    virtual ~Orocos_Method_i (void) {}
  
  virtual
  CORBA::Boolean execute (
      
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
      )) {
      return this->evaluate();
  }

  virtual
  void reset(
      
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
      )) {
      this->mmethod->reset();
  }
};

class  Orocos_AnyMethod_i
    : public virtual Orocos_AnyExpression_i,
      public virtual POA_Orocos::Method,
      public virtual PortableServer::RefCountServantBase
{
public:
    ORO_CoreLib::DataSourceBase::shared_ptr mmethod;

    virtual void copy( ORO_CoreLib::DataSourceBase::shared_ptr new_ds ) {
        mmethod = new_ds;
        morig = new_ds;
    }

  //Constructor 
  Orocos_AnyMethod_i ( ORO_CoreLib::DataSourceBase::shared_ptr datas )
      : Orocos_AnyExpression_i( datas ), mmethod( datas )
    {}
  
  //Destructor 
    virtual ~Orocos_AnyMethod_i (void) {}
  
  virtual
  CORBA::Boolean execute (
      
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
      )) {
      return this->evaluate();
  }

  virtual
  void reset(
      
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
      )) {
      this->mmethod->reset();
  }
};


class  Orocos_Command_i : public virtual POA_Orocos::Command, public virtual PortableServer::RefCountServantBase
{
    ORO_Execution::CommandC* morig;
public:
  //Constructor 
  Orocos_Command_i (ORO_Execution::CommandC& c);
  
  //Destructor 
  virtual ~Orocos_Command_i (void);
  
  virtual
  CORBA::Boolean execute (
      
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
    ));
  
  virtual
  CORBA::Boolean sent (
      
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
    ));
  
  virtual
  CORBA::Boolean accepted (
      
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
    ));
  
  virtual
  CORBA::Boolean executed (
      
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
    ));
  
  virtual
  CORBA::Boolean valid (
      
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
    ));
  
  virtual
  CORBA::Boolean evaluate (
      
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
    ));
  
  virtual
  void reset (
      
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
    ));
  
  virtual
  ::Orocos::Action_ptr createAction (
      
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
    ));
  
  virtual
  ::Orocos::Expression_ptr createCondition (
      
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
    ));
};

#endif /* INCLUDE_EXPRESSIONI_H_  */

