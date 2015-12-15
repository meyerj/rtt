/***************************************************************************
  tag: FMTC  Tue Mar 11 21:49:24 CET 2008  CorbaTemplateProtocol.hpp

                        CorbaTemplateProtocol.hpp -  description
                           -------------------
    begin                : Tue March 11 2008
    copyright            : (C) 2008 FMTC
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


#ifndef ORO_CORBA_TEMPATE_PROTOCOL_HPP
#define ORO_CORBA_TEMPATE_PROTOCOL_HPP

#include "CorbaTypeTransporter.hpp"
#include "RemoteChannelElement.hpp"
#include "../../types/Types.hpp"
#include "../../InputPort.hpp"
#include "../../OutputPort.hpp"
#include "ServiceC.h"
#include "DataFlowI.h"
#include "../../internal/ConnID.hpp"
#include "DataSourceProxy.hpp"

namespace RTT
{ namespace corba
  {
      /**
       * For each transportable type T, specify the conversion functions.
       */
      template<class T>
      class CorbaTemplateProtocol
          : public CorbaTypeTransporter
      {
      public:
          /**
           * The given \a T parameter is the type for reading DataSources.
           */
          typedef T UserType;
          /**
           * When Properties of \a T are constructed, they are non-const, non-reference.
           */
          typedef typename Property<T>::DataSourceType PropertyType;

          CRemoteChannelElement_i* createChannelElement_i(DataFlowInterface* sender,PortableServer::POA_ptr poa, bool is_pull, bool is_signalling) const
          { return new RemoteChannelElement<T>(*this, sender, poa, is_pull, is_signalling); }

          /**
           * Create an transportable object for a \a protocol which contains the value of \a source.
           */
          virtual CORBA::Any* createAny( base::DataSourceBase::shared_ptr source) const
          {
              typename internal::LateReferenceDataSource<T>::shared_ptr d_ref = boost::dynamic_pointer_cast< internal::LateReferenceDataSource<T> >( source );
              if ( d_ref )
                  return AnyConversion<PropertyType>::createAny( d_ref->set());

              typename internal::DataSource<T>::shared_ptr d = boost::dynamic_pointer_cast< internal::DataSource<T> >( source );
              if ( d )
                  return AnyConversion<PropertyType>::createAny( d->get());

              return 0;
          }

          /**
           * Update a transportable object for a \a protocol which contains the value of \a source.
           */
          virtual bool updateAny( base::DataSourceBase::shared_ptr source, CORBA::Any& any) const
          {
              typename internal::LateConstReferenceDataSource<T>::shared_ptr d_ref = boost::dynamic_pointer_cast< internal::LateConstReferenceDataSource<T> >( source );
              if ( d_ref )
                  return AnyConversion<PropertyType>::updateAny( d_ref->rvalue(), any);

              typename internal::DataSource<T>::shared_ptr d = boost::dynamic_pointer_cast< internal::DataSource<T> >( source );
              if ( d )
                  return AnyConversion<PropertyType>::updateAny( d->get(), any);

              return false;
          }

          virtual base::DataSourceBase::shared_ptr createDataSource(const CORBA::Any* any) const
          {
              base::DataSourceBase::shared_ptr result = new internal::ValueDataSource<T>();
              if ( updateFromAny(any, result) )
                  return result;
              return base::DataSourceBase::shared_ptr();
          }

          /**
           * Update \a target with the contents of \a any which is an object of a \a protocol.
           */
          virtual bool updateFromAny(const CORBA::Any* any, base::DataSourceBase::shared_ptr target) const
          {
            typename internal::LateReferenceDataSource<T>::shared_ptr ad_ref = boost::dynamic_pointer_cast< internal::LateReferenceDataSource<T> >( target );
            if ( ad_ref ) {
                if (AnyConversion<PropertyType>::update(*any, ad_ref->set() ) ) {
                    return true;
                }
                return false;
            }

            typename internal::AssignableDataSource<T>::shared_ptr ad = internal::AssignableDataSource<T>::narrow( target.get() );
            if ( ad ) {
                PropertyType value = PropertyType();
                if (AnyConversion<PropertyType>::update(*any, value ) ) {
                    ad->set( value );
                    return true;
                }
                return false;
            }

            return false;
          }

          virtual base::DataSourceBase::shared_ptr createPropertyDataSource(CService_ptr serv, const std::string& vname) {
              return base::DataSourceBase::shared_ptr( new ValueDataSourceProxy<PropertyType>( serv, vname, true) );
          }

          virtual base::DataSourceBase::shared_ptr createAttributeDataSource(CService_ptr serv, const std::string& vname, bool is_assignable) {
              if ( is_assignable ) {
                  return base::DataSourceBase::shared_ptr( new ValueDataSourceProxy<PropertyType>( serv, vname, false) );
              }
              else {
                  return base::DataSourceBase::shared_ptr( new DataSourceProxy<PropertyType>( serv, vname, false ) );
              }
          }
      };
}
}

#endif
