/***************************************************************************
  tag: Peter Soetens  Thu Oct 22 11:59:07 CEST 2009  DataObjectUnSync.hpp

                        DataObjectUnSync.hpp -  description
                           -------------------
    begin                : Thu October 22 2009
    copyright            : (C) 2009 Peter Soetens
    email                : peter@thesourcworks.com

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



#ifndef CORELIB_DATAOBJECT_UNSYNC_HPP
#define CORELIB_DATAOBJECT_UNSYNC_HPP


#include "DataObjectInterface.hpp"

namespace RTT
{ namespace base {

    /**
     * @brief A class which provides unprotected (\b not thread-safe) access to one typed element of data.
     *
     * @ingroup PortBuffers
     */
    template<class T>
    class DataObjectUnSync
        : public DataObjectInterface<T>
    {
        /**
         * One element of Data.
         */
        T data;

        mutable FlowStatus status;
        bool initialized;

    public:
        /**
         * Construct an uninitialized DataObjectUnSync.
         */
        DataObjectUnSync()
            : data(), status(NoData), initialized(false) {}

        /**
         * Construct a DataObjectUnSync with initial value.
         */
        DataObjectUnSync( const T& initial_value )
            : data(initial_value), status(NoData), initialized(true) {}

        /**
         * The type of the data.
         */
        typedef T DataType;

        virtual FlowStatus Get( DataType& pull, bool copy_old_data = true ) const {
            FlowStatus result = status;
            if (status == NewData) {
                pull = data;
                status = OldData;
            } else if (copy_old_data) {
                pull = data;
            }
            return result;
        }

        virtual DataType Get() const {
            DataType cache;
            Get(cache);
            return cache;
        }

        virtual bool Set( const DataType& push ) {
            data = push;
            status = NewData;
            return true;
        }

        virtual bool data_sample( const DataType& sample, bool reset ) {
            if (!initialized || reset) {
                Set(sample);
                initialized = true;
                return true;
            } else {
                return initialized;
            }
        }

        virtual T data_sample() const
        {
            return data;
        }

        virtual void clear()
        {
            status = NoData;
        }

    };
}}

#endif

