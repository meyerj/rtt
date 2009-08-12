/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:25:58 CEST 2006  ExecutionI.h

                        ExecutionI.h -  description
                           -------------------
    begin                : Mon June 26 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@fmtc.be

 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public                   *
 *   License as published by the Free Software Foundation;                 *
 *   version 2 of the License.                                             *
 *                                                                         *
 *   As a special exception, you may use this file as part of a free       *
 *   software library without restriction.  Specifically, if other files   *
 *   instantiate templates or use macros or inline functions from this     *
 *   file, or you compile this file and link it with other files to        *
 *   produce an executable, this file does not by itself cause the         *
 *   resulting executable to be covered by the GNU General Public          *
 *   License.  This exception does not however invalidate any other        *
 *   reasons why the executable file might be covered by the GNU General   *
 *   Public License.                                                       *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
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
#include "../rtt-config.h"
#include "corba.h"
#include "../../internal/DataSource.hpp"

#ifndef INCLUDE_EXECUTIONI_H_
#define INCLUDE_EXECUTIONI_H_

#include "OperationsC.h"
#ifdef CORBA_IS_TAO
#include "OperationsS.h"
#endif
#include "CorbaConversion.hpp"
#include "AnyDataSource.hpp"
#include "../../base/DispatchInterface.hpp"
#include "../../Logger.hpp"
#include "../../internal/MethodC.hpp"
#include "../../internal/DataSources.hpp"

namespace RTT
{
    class CommandC;
}

#if !defined (ACE_LACKS_PRAGMA_ONCE)
#pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

/**
 * A servant which serves a RTT::DataSource through the 'Any' methods.
 */
class  Orocos_AnyExpression_i
  : public virtual POA_RTT::Corba::Expression,
    public virtual PortableServer::RefCountServantBase
{
protected:
    RTT::DataSourceBase::shared_ptr morig;
    CORBA::Any_var last_value;
    PortableServer::POA_var mpoa;
public:
    typedef RTT::DataSourceBase::const_ptr SourceType;
    typedef CORBA::Any                             ResultType;

    virtual void copy( RTT::DataSourceBase::shared_ptr new_ds ) {
        morig = new_ds;
    }

  // Constructor
  Orocos_AnyExpression_i (RTT::DataSourceBase::shared_ptr orig, PortableServer::POA_ptr the_poa)
      : morig( orig ), last_value( (CORBA::Any_ptr)morig->createBlob(ORO_CORBA_PROTOCOL_ID) ) // create default Any.
        , mpoa( PortableServer::POA::_duplicate(the_poa) )
    {}

    PortableServer::POA_ptr _default_POA()
    {
        return PortableServer::POA::_duplicate(mpoa);
    }

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
      last_value = (CORBA::Any_ptr)morig->getBlob(ORO_CORBA_PROTOCOL_ID);
      bool result = true;
      // if it is a bool, update result and return it, otherwise, just return true:
      RTT::AnyConversion<bool>::update( last_value.in(), result );
      return result;
  }

  virtual
  CORBA::Any * get (

    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
      )) {
      return (CORBA::Any*)morig->getBlob(ORO_CORBA_PROTOCOL_ID);
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
  char * toString (

    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
      )) {
      std::string result = morig->toString();
      return CORBA::string_dup( result.c_str() );
  }


  virtual
  char * getTypeName (

    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
      )) {
      return CORBA::string_dup( morig->getTypeName().c_str() );
  }

  virtual void destroyExpression()
    ACE_THROW_SPEC ((
      CORBA::SystemException
    ))
 {
  }

};


class  Orocos_AnyAssignableExpression_i
    : public Orocos_AnyExpression_i,
      public virtual POA_RTT::Corba::AssignableExpression
{
    RTT::DataSourceBase::shared_ptr mset;
public:

    virtual void copy( RTT::DataSourceBase::shared_ptr new_ds ) {
        mset = new_ds;
        morig = new_ds;
    }

  // Constructor
  Orocos_AnyAssignableExpression_i (RTT::DataSourceBase::shared_ptr orig, PortableServer::POA_ptr the_poa)
      : Orocos_AnyExpression_i(orig, the_poa), mset( orig )
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
      return mset->updateBlob(ORO_CORBA_PROTOCOL_ID, &value );
  }

  virtual
  CORBA::Boolean fromString (
      const char* value
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
      )) {

      if ( !mset->getTypeInfo()->fromString( value, mset ) ) {
          RTT::log(RTT::Error) << "Corba::AssignableExpression: Could not assign string to "<<mset->getType() <<"." <<RTT::endlog()
							   <<" Tried to assign as "<< RTT::DataSource<ResultType>::GetType() << " to native type "<< RTT::DataSource<SourceType>::GetType()<< RTT::endlog();
          return false;
      }
      return true;
  }

};


class  Orocos_Action_i
    : public virtual POA_RTT::Corba::Action,
      public virtual PortableServer::RefCountServantBase
{
	RTT::MethodC morig;
    RTT::CommandInterface* mcom;
    PortableServer::POA_var mpoa;
public:
  //Constructor
  Orocos_Action_i ( RTT::MethodC* orig, RTT::CommandInterface* com, PortableServer::POA_ptr the_poa );

    PortableServer::POA_ptr _default_POA()
    {
        return PortableServer::POA::_duplicate(mpoa);
    }

  //Destructor
    virtual ~Orocos_Action_i (void);

  virtual
  CORBA::Boolean execute (

    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
      ));

  virtual
  CORBA::Boolean executeAny (
      const ::RTT::Corba::AnyArguments& args
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
    ,::RTT::Corba::WrongNumbArgException
    ,::RTT::Corba::WrongTypeArgException
    ));

  virtual
  void reset (

    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
      ));

  virtual void destroyAction()
    ACE_THROW_SPEC ((
      CORBA::SystemException
    ))
 {
  }

};

class  Orocos_AnyMethod_i
    : public virtual Orocos_AnyExpression_i,
      public virtual POA_RTT::Corba::Method
{
protected:
	RTT::MethodC mmethodc;

public:
    virtual void copy( RTT::DataSourceBase::shared_ptr new_ds ) {
        morig = new_ds;
    }

  //Constructor
  Orocos_AnyMethod_i (RTT::MethodC orig, RTT::DataSourceBase::shared_ptr datas, PortableServer::POA_ptr the_poa )
      : Orocos_AnyExpression_i( datas, the_poa ), mmethodc(orig)
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

  CORBA::Boolean executeAny (
      const ::RTT::Corba::AnyArguments& args
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
    ,::RTT::Corba::WrongNumbArgException
    ,::RTT::Corba::WrongTypeArgException
	  )) {
      RTT::MethodC mgen = mmethodc;
    try {
        for (size_t i =0; i != args.length(); ++i)
            mgen.arg( RTT::DataSourceBase::shared_ptr( new RTT::Corba::AnyDataSource( new CORBA::Any( args[i] ) )));
        // if not ready, not enough args were given, *guess* a one off error in the exception :-(
        if ( !mgen.ready() )
            throw ::RTT::Corba::WrongNumbArgException( args.length()+1, args.length() );
        morig = mgen.getDataSource();
        return this->evaluate();
    } catch ( RTT::wrong_number_of_args_exception& wna ) {
        throw ::RTT::Corba::WrongNumbArgException( wna.wanted, wna.received );
    } catch ( RTT::wrong_types_of_args_exception& wta ) {
        throw ::RTT::Corba::WrongTypeArgException( wta.whicharg, wta.expected_.c_str(), wta.received_.c_str() );
    }
    return false;
  }

  virtual
  void reset(

    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
      )) {
      this->morig->reset();
  }

  virtual void destroyAction()
    ACE_THROW_SPEC ((
      CORBA::SystemException
    ))
 {
  }

};


class  Orocos_Command_i
    : public virtual POA_RTT::Corba::Command,
      public virtual PortableServer::RefCountServantBase
{
protected:
    RTT::CommandC* morig;
    RTT::CommandC* mcomm;
    PortableServer::POA_var mpoa;
public:
  //Constructor
    /**
     * Create a CORBA servant for a CommandC object.
     * @param orig A CommandC object being ready for receiving arg() calls. This is required
     * for executeAny(), such that new arguments can be provided.
     * @param c A CommandC object being ready for execute(). This is required for execute().
     */
  Orocos_Command_i (RTT::CommandC& orig,RTT::CommandC& c, PortableServer::POA_ptr the_poa);

  /**
   * Create a CORBA servant for a CommandC object without executeAny() support.
   */
  Orocos_Command_i (RTT::DispatchInterface::shared_ptr orig, PortableServer::POA_ptr the_poa);

    PortableServer::POA_ptr _default_POA()
    {
        return PortableServer::POA::_duplicate(mpoa);
    }

  //Destructor
  virtual ~Orocos_Command_i (void);

  virtual
  CORBA::Boolean execute (

    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
    ));

  virtual
  CORBA::Boolean executeAny (
      const ::RTT::Corba::AnyArguments& args
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
    ,::RTT::Corba::WrongNumbArgException
    ,::RTT::Corba::WrongTypeArgException
    ));

  virtual
  RTT::Corba::CommandStatus status ()
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
  CORBA::Boolean done (

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

  virtual void destroyCommand()
    ACE_THROW_SPEC ((
      CORBA::SystemException
    ));
};

#endif /* INCLUDE_EXPRESSIONI_H_  */

