/***************************************************************************
  tag: Peter Soetens  Mon Jan 10 15:59:51 CET 2005  buffers_test.cpp

                        buffers_test.cpp -  description
                           -------------------
    begin                : Mon January 10 2005
    copyright            : (C) 2005 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "unit.hpp"

#include <iostream>
#include <boost/scoped_ptr.hpp>

#include <internal/AtomicQueue.hpp>
#include <internal/AtomicMWSRQueue.hpp>

#include <Activity.hpp>

#include <RTT.hpp>
#include <base/Buffer.hpp>
#include <internal/ListLockFree.hpp>
#include <base/DataObject.hpp>
#include <internal/TsPool.hpp>
//#include <internal/SortedList.hpp>

#include <os/Thread.hpp>
#include <rtt-config.h>

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace RTT;
using namespace RTT::detail;

class Dummy {
public:
    Dummy(double a = 0.0, double b =1.0, double c=2.0)
        :d1(a), d2(b), d3(c) {}
    double d1;
    double d2;
    double d3;
    bool operator==(const Dummy& d) const
    {
        return d.d1 == d1 && d.d2 == d2 && d.d3 == d3;
    }

    bool operator!=(const Dummy& d) const
    {
        return d.d1 != d1 || d.d2 != d2 || d.d3 != d3;
    }

    bool operator<(const Dummy& d) const
    {
        return d1+d2+d3 < d.d1 + d.d2 + d.d3;
    }
/*
     volatile Dummy& operator=(const Dummy& d) volatile
     {
         d1 = d.d1;
         d2 = d.d2;
         d3 = d.d3;
         return *this;
     }
*/
};


typedef AtomicMWMRQueue<Dummy*> MWMRQueueType;
typedef AtomicMWSRQueue<Dummy*> MWSRQueueType;

// Don't make queue size too large, we want to catch
// overrun issues too.
#define QS 10

class BuffersAtomicMWMRQueueTest
{
public:
    MWMRQueueType* aqueue;
    ThreadInterface* athread;
    ThreadInterface* bthread;
    ListLockFree<Dummy>* listlockfree;

    BuffersAtomicMWMRQueueTest()
    {
        aqueue = new MWMRQueueType(QS);
        listlockfree = new ListLockFree<Dummy>(10, 4);
    }
    ~BuffersAtomicMWMRQueueTest(){
        aqueue->clear();
        delete aqueue;
        delete listlockfree;
    }
};

class BuffersAtomicMWSRQueueTest
{
public:
    MWSRQueueType* aqueue;

    BuffersAtomicMWSRQueueTest()
    {
        aqueue = new MWSRQueueType(QS);
    }
    ~BuffersAtomicMWSRQueueTest(){
        aqueue->clear();
        delete aqueue;
    }
};

template <class Worker>
class ThreadPool : public std::vector< std::pair< boost::shared_ptr<Worker>, boost::shared_ptr<ThreadInterface> > >
{
public:
    typedef std::vector< std::pair< boost::shared_ptr<Worker>, boost::shared_ptr<ThreadInterface> > > Threads;
    typedef typename Threads::value_type value_type;
    typedef typename Threads::iterator iterator;
    typedef typename Threads::const_iterator const_iterator;

    template <typename Arg1>
    ThreadPool(int threads, int scheduler, int priority, Seconds period, const std::string &name, const Arg1 &arg1)
        : Threads(threads)
    {
        int count = 0;
        for(iterator worker = this->begin(); worker != this->end(); ++worker) {
            worker->first.reset(new Worker(arg1));
            worker->second.reset(new Activity(scheduler, priority, period, worker->first.get(), name + boost::lexical_cast<std::string>(count++)));
        }
    }

    ~ThreadPool()
    {
        stop();
        for(iterator worker = this->begin(); worker != this->end(); ++worker) {
            worker->second.reset();
            worker->first.reset();
        }
    }

    bool start()
    {
        bool result = true;
        for(const_iterator worker = this->begin(); worker != this->end(); ++worker) {
            if (!worker->second->start()) result = false;
        }
        return result;
    }

    bool stop()
    {
        bool result = true;
        for(const_iterator worker = this->begin(); worker != this->end(); ++worker) {
            worker->second->stop();
        }
        return result;
    }
};

class BuffersDataFlowTest
{
public:
    BufferInterface<Dummy>* buffer;
    BufferInterface<Dummy>* circular;
    DataObjectInterface<Dummy>* dataobj;

    BufferLockFree<Dummy>* lockfree;
    BufferLocked<Dummy>* locked;
    BufferUnSync<Dummy>* unsync;

    BufferLockFree<Dummy>* clockfree;
    BufferLocked<Dummy>* clocked;
    BufferUnSync<Dummy>* cunsync;

    DataObjectLocked<Dummy>* dlocked;
    DataObjectLockFree<Dummy>* dlockfree;
    DataObjectUnSync<Dummy>* dunsync;

    void testBuf();
    void testCirc();
    void testDObj();

    typedef std::map<FlowStatus, int> ReadsByStatusMap;

    void testBufMultiThreaded(int number_of_writers, int number_of_readers);
    void testDObjMultiThreaded(int number_of_writers, int number_of_readers);

    BuffersDataFlowTest()
    {
        // classical variants
        lockfree = new BufferLockFree<Dummy>(QS, Dummy());
        locked = new BufferLocked<Dummy>(QS, Dummy());
        unsync = new BufferUnSync<Dummy>(QS, Dummy());

        // circular variants.
        clockfree = new BufferLockFree<Dummy>(QS, Dummy(), BufferBase::Options().circular(true));
        clocked = new BufferLocked<Dummy>(QS, Dummy(), BufferBase::Options().circular(true));
        cunsync = new BufferUnSync<Dummy>(QS, Dummy(), BufferBase::Options().circular(true));

        dlockfree = new DataObjectLockFree<Dummy>(Dummy());
        dlocked   = new DataObjectLocked<Dummy>(Dummy());
        dunsync   = new DataObjectUnSync<Dummy>(Dummy());

        // defaults
        buffer = lockfree;
        dataobj = dlockfree;
    }

    ~BuffersDataFlowTest(){
        delete lockfree;
        delete locked;
        delete unsync;
        delete clockfree;
        delete clocked;
        delete cunsync;
        delete dlockfree;
        delete dlocked;
        delete dunsync;
    }

    class DataObjectWriter : public RunnableInterface {
    private:
        DataObjectInterface<Dummy> *dataobj;
        bool stop;
        Dummy sample;

    public:
        int writes;
        int dropped;

    public:
        DataObjectWriter(DataObjectInterface<Dummy> *dataobj) : dataobj(dataobj), stop(false), writes(0), dropped(0) {}
        bool initialize() {
            stop = false;
            return true;
        }
        void step() {
            while (stop == false) {
                if (dataobj->Set(sample)) {
                    ++writes;
                } else {
                    ++dropped;
                }
            }
        }

        void finalize() {}

        bool breakLoop() {
            stop = true;
            return true;
        }
    };

    class DataObjectReader : public RunnableInterface {
    private:
        DataObjectInterface<Dummy> *dataobj;
        bool stop;
        Dummy sample;

    public:
        int reads;
        ReadsByStatusMap reads_by_status;

    public:
        DataObjectReader(DataObjectInterface<Dummy> *dataobj) : dataobj(dataobj), stop(false), reads(0) {}
        bool initialize() {
            stop = false;
            return true;
        }
        void step() {
            while (stop == false) {
                FlowStatus fs = dataobj->Get(sample, false);
                ++reads;
                ++reads_by_status[fs];
            }
        }

        void finalize() {}

        bool breakLoop() {
            stop = true;
            return true;
        }
    };

    class BufferWriter : public RunnableInterface {
    private:
        BufferInterface<Dummy> *buffer;
        bool stop;
        Dummy sample;

    public:
        int writes;
        int dropped;

    public:
        BufferWriter(BufferInterface<Dummy> *buffer) : buffer(buffer), stop(false), writes(0), dropped(0) {}
        bool initialize() {
            stop = false;
            return true;
        }
        void step() {
            while (stop == false) {
                if (buffer->Push(sample)) {
                    ++writes;
                } else {
                    ++dropped;
                }
            }
        }

        void finalize() {}

        bool breakLoop() {
            stop = true;
            return true;
        }
    };

    class BufferReader : public RunnableInterface {
    private:
        BufferInterface<Dummy> *buffer;
        bool stop;
        Dummy sample;

    public:
        int reads;
        ReadsByStatusMap reads_by_status;

    public:
        BufferReader(BufferInterface<Dummy> *buffer) : buffer(buffer), stop(false), reads(0) {}
        bool initialize() {
            stop = false;
            return true;
        }
        void step() {
            while (stop == false) {
                FlowStatus fs = buffer->Pop(sample);
                ++reads;
                ++reads_by_status[fs];
            }
        }

        void finalize() {}

        bool breakLoop() {
            stop = true;
            return true;
        }
    };
};

void BuffersDataFlowTest::testBuf()
{
    /**
     * Single Threaded test for BufferLockFree.
     * This is a mixed white/black box test.
     */

    Dummy* d = new Dummy;
    Dummy* c = new Dummy(2.0, 1.0, 0.0);
    Dummy r;

    BOOST_CHECK( buffer->Pop(r) == false );

    BOOST_CHECK( buffer->Push( *d ) );
    BOOST_CHECK( buffer->Pop(r) );
    BOOST_CHECK( r == *d );

    BOOST_CHECK( buffer->Push( *c ) );
    BOOST_CHECK( buffer->Pop(r) );
    BOOST_CHECK( r == *c );

    BOOST_CHECK( buffer->Push( *d ) );
    BOOST_CHECK( buffer->Push( *c ) );
    BOOST_CHECK( buffer->Push( *d ) );
    BOOST_CHECK( buffer->Push( *c ) );
    BOOST_CHECK( buffer->Push( *d ) );
    BOOST_CHECK( buffer->Push( *c ) );
    BOOST_CHECK( buffer->Push( *d ) );
    BOOST_CHECK( buffer->Push( *c ) );
    BOOST_CHECK( buffer->Push( *d ) );
    BOOST_CHECK( buffer->Push( *c ) );
    BOOST_CHECK( buffer->Push( *c ) == false );
    BOOST_CHECK( buffer->Push( *c ) == false );
    BOOST_CHECK( buffer->Push( *c ) == false );
    BOOST_CHECK( buffer->Push( *c ) == false );
    BOOST_CHECK( buffer->Push( *c ) == false );
    BOOST_CHECK( buffer->Push( *c ) == false );
    BOOST_CHECK( buffer->Push( *c ) == false );
    BOOST_CHECK( buffer->Push( *c ) == false );
    BOOST_CHECK( buffer->Push( *c ) == false );
    BOOST_CHECK( buffer->Push( *c ) == false );
    BOOST_CHECK( buffer->Push( *c ) == false );
    BOOST_CHECK( buffer->Push( *c ) == false );
    BOOST_CHECK( buffer->Pop(r) );
    BOOST_CHECK( r == *d );
    BOOST_CHECK( buffer->Pop(r) );
    BOOST_CHECK( r == *c );
    BOOST_CHECK( buffer->Pop(r) );
    BOOST_CHECK( r == *d );
    BOOST_CHECK( buffer->Pop(r) );
    BOOST_CHECK( r == *c );
    BOOST_CHECK( buffer->Pop(r) );
    BOOST_CHECK( r == *d );

    // start writing again half-way
    BOOST_CHECK( buffer->Push( *d ) );
    BOOST_CHECK( buffer->Push( *c ) );
    BOOST_CHECK( buffer->Push( *d ) );
    BOOST_CHECK( buffer->Push( *c ) );
    BOOST_CHECK( buffer->Push( *d ) );

    BOOST_CHECK( buffer->Pop(r) );
    BOOST_CHECK( r == *c );
    BOOST_CHECK( buffer->Pop(r) );
    BOOST_CHECK( r == *d );
    BOOST_CHECK( buffer->Pop(r) );
    BOOST_CHECK( r == *c );
    BOOST_CHECK( buffer->Pop(r) );
    BOOST_CHECK( r == *d );
    BOOST_CHECK( buffer->Pop(r) );
    BOOST_CHECK( r == *c );
    BOOST_CHECK( buffer->Pop(r) );
    BOOST_CHECK( r == *d );
    BOOST_CHECK( buffer->Pop(r) );
    BOOST_CHECK( r == *c );
    BOOST_CHECK( buffer->Pop(r) );
    BOOST_CHECK( r == *d );
    BOOST_CHECK( buffer->Pop(r) );
    BOOST_CHECK( r == *c );
    BOOST_CHECK( buffer->Pop(r) );
    BOOST_CHECK( r == *d );

    BOOST_CHECK( buffer->Pop(r) == false );
    BOOST_CHECK( buffer->Pop(r) == false );
    BOOST_CHECK( buffer->Pop(r) == false );
    BOOST_CHECK( buffer->Pop(r) == false );
    BOOST_CHECK( buffer->Pop(r) == false );
    BOOST_CHECK( buffer->Pop(r) == false );
    BOOST_CHECK( buffer->Pop(r) == false );
    BOOST_CHECK( buffer->Pop(r) == false );
    BOOST_CHECK( buffer->Pop(r) == false );
    BOOST_CHECK( buffer->Pop(r) == false );
    BOOST_CHECK( buffer->Pop(r) == false );

    BOOST_CHECK( buffer->Push( *c ) );
    BOOST_CHECK( buffer->Push( *d ) );
    BOOST_CHECK( buffer->Push( *c ) );
    BOOST_CHECK( buffer->Push( *d ) );
    BOOST_CHECK( buffer->Push( *c ) );

    std::vector<Dummy> v;
    BOOST_CHECK( 5 == buffer->Pop(v) );
    BOOST_CHECK( v[0] == *c );
    BOOST_CHECK( v[1] == *d );
    BOOST_CHECK( v[2] == *c );
    BOOST_CHECK( v[3] == *d );
    BOOST_CHECK( v[4] == *c );

    BufferBase::size_type sz = 10;
    BOOST_CHECK( buffer->Push( *c ) );
    BOOST_CHECK( buffer->Push( *d ) );
    BOOST_CHECK( buffer->Push( v ) == (int)v.size() );
    BOOST_CHECK( buffer->Push( *c ) );
    BOOST_CHECK( buffer->Push( *d ) );
    BOOST_CHECK( buffer->Push( v ) == 1 );
    BOOST_CHECK( buffer->Push( v ) == 0 );
    BOOST_CHECK( buffer->Push( v ) == 0 );
    BOOST_CHECK( buffer->Push( v ) == 0 );
    BOOST_CHECK( buffer->Push( v ) == 0 );
    BOOST_CHECK( buffer->Push( v ) == 0 );
    BOOST_CHECK( buffer->Push( v ) == 0 );
    BOOST_CHECK( buffer->Push( v ) == 0 );
    BOOST_CHECK( buffer->Push( v ) == 0 );
    BOOST_CHECK( buffer->Push( v ) == 0 );
    BOOST_CHECK( buffer->Push( v ) == 0 );
    BOOST_CHECK( buffer->Push( v ) == 0 );
    BOOST_CHECK( buffer->Push( v ) == 0 );
    BOOST_REQUIRE_EQUAL( sz, buffer->Pop(v) );
    BOOST_CHECK( v[0] == *c );
    BOOST_CHECK( v[1] == *d );
    BOOST_CHECK( v[2] == *c );
    BOOST_CHECK( v[3] == *d );
    BOOST_CHECK( v[4] == *c );
    BOOST_CHECK( v[5] == *d );
    BOOST_CHECK( v[6] == *c );
    BOOST_CHECK( v[7] == *c );
    BOOST_CHECK( v[8] == *d );
    //BOOST_CHECK( v[9] == *c );
    BOOST_CHECK( 0 == buffer->Pop(v) );
    delete d;
    delete c;
}

void BuffersDataFlowTest::testCirc()
{
    /**
     * Single Threaded test for a circular BufferLockFree.
     * This is a mixed white/black box test.
     */

    Dummy* d = new Dummy;
    Dummy* c = new Dummy(2.0, 1.0, 0.0);
    Dummy r;

    BOOST_CHECK( circular->Pop(r) == false );

    BOOST_CHECK( circular->Push( *d ) );
    BOOST_CHECK( circular->Pop(r) );
    BOOST_CHECK( r == *d );

    BOOST_CHECK( circular->Push( *c ) );
    BOOST_CHECK( circular->Pop(r) );
    BOOST_CHECK( r == *c );

    BOOST_CHECK( circular->Push( *d ) );
    BOOST_CHECK( circular->Push( *c ) );
    BOOST_CHECK( circular->Push( *d ) );
    BOOST_CHECK( circular->Push( *c ) );
    BOOST_CHECK( circular->Push( *d ) );
    BOOST_CHECK( circular->Push( *c ) );
    BOOST_CHECK( circular->Push( *d ) );
    BOOST_CHECK( circular->Push( *c ) );
    BOOST_CHECK( circular->Push( *d ) );
    BOOST_CHECK( circular->Push( *c ) ); // oldest item at end of Push series.
    BOOST_CHECK( circular->Push( *d ) );
    BOOST_CHECK( circular->Push( *c ) );
    BOOST_CHECK( circular->Push( *d ) );
    BOOST_CHECK( circular->Push( *c ) );
    BOOST_CHECK( circular->Push( *d ) );
    BOOST_CHECK( circular->Push( *c ) );
    BOOST_CHECK( circular->Push( *d ) );
    BOOST_CHECK( circular->Push( *c ) );
    BOOST_CHECK( circular->Push( *d ) );
    BOOST_CHECK( circular->Pop(r) );
    BOOST_CHECK( r == *c );
    BOOST_CHECK( circular->Pop(r) );
    BOOST_CHECK( r == *d );
    BOOST_CHECK( circular->Pop(r) );
    BOOST_CHECK( r == *c );
    BOOST_CHECK( circular->Pop(r) );
    BOOST_CHECK( r == *d );
    BOOST_CHECK( circular->Pop(r) );
    BOOST_CHECK( r == *c );

    // start writing again half-way
    BOOST_CHECK( circular->Push( *d ) );
    BOOST_CHECK( circular->Push( *c ) );
    BOOST_CHECK( circular->Push( *d ) );
    BOOST_CHECK( circular->Push( *c ) );
    BOOST_CHECK( circular->Push( *d ) );

    BOOST_CHECK( circular->Pop(r) );
    BOOST_CHECK( r == *d );
    BOOST_CHECK( circular->Pop(r) );
    BOOST_CHECK( r == *c );
    BOOST_CHECK( circular->Pop(r) );
    BOOST_CHECK( r == *d );
    BOOST_CHECK( circular->Pop(r) );
    BOOST_CHECK( r == *c );
    BOOST_CHECK( circular->Pop(r) );
    BOOST_CHECK( r == *d );
    BOOST_CHECK( circular->Pop(r) );
    BOOST_CHECK( r == *d );
    BOOST_CHECK( circular->Pop(r) );
    BOOST_CHECK( r == *c );
    BOOST_CHECK( circular->Pop(r) );
    BOOST_CHECK( r == *d );
    BOOST_CHECK( circular->Pop(r) );
    BOOST_CHECK( r == *c );
    BOOST_CHECK( circular->Pop(r) );
    BOOST_CHECK( r == *d );

    BOOST_CHECK( circular->Pop(r) == false );
    BOOST_CHECK( circular->Pop(r) == false );
    BOOST_CHECK( circular->Pop(r) == false );
    BOOST_CHECK( circular->Pop(r) == false );
    BOOST_CHECK( circular->Pop(r) == false );
    BOOST_CHECK( circular->Pop(r) == false );
    BOOST_CHECK( circular->Pop(r) == false );
    BOOST_CHECK( circular->Pop(r) == false );
    BOOST_CHECK( circular->Pop(r) == false );
    BOOST_CHECK( circular->Pop(r) == false );
    BOOST_CHECK( circular->Pop(r) == false );

    BOOST_CHECK( circular->Push( *c ) );
    BOOST_CHECK( circular->Push( *d ) );
    BOOST_CHECK( circular->Push( *c ) );
    BOOST_CHECK( circular->Push( *d ) );
    BOOST_CHECK( circular->Push( *c ) );

    std::vector<Dummy> v;
    BOOST_CHECK( 5 == circular->Pop(v) );
    BOOST_CHECK( v[0] == *c );
    BOOST_CHECK( v[1] == *d );
    BOOST_CHECK( v[2] == *c );
    BOOST_CHECK( v[3] == *d );
    BOOST_CHECK( v[4] == *c );

    BufferBase::size_type sz = 10;
    BOOST_CHECK( circular->Push( *c ) );
    BOOST_CHECK( circular->Push( *d ) );
    BOOST_CHECK( circular->Push( v ) == (int)v.size() );
    BOOST_CHECK( circular->Push( *c ) );
    BOOST_CHECK( circular->Push( *d ) );
    BOOST_CHECK( circular->Push( v ) == (int)v.size() );
    BOOST_CHECK( circular->Push( v ) == (int)v.size() );
    BOOST_CHECK( circular->Push( v ) == (int)v.size() );
    BOOST_CHECK( circular->Push( v ) == (int)v.size() );
    BOOST_CHECK( circular->Push( v ) == (int)v.size() );
    BOOST_REQUIRE_EQUAL( sz, circular->Pop(v) );
    BOOST_CHECK( v[0] == *c );
    BOOST_CHECK( v[1] == *d );
    BOOST_CHECK( v[2] == *c );
    BOOST_CHECK( v[3] == *d );
    BOOST_CHECK( v[4] == *c );
    BOOST_CHECK( v[5] == *c );
    BOOST_CHECK( v[6] == *d );
    BOOST_CHECK( v[7] == *c );
    BOOST_CHECK( v[8] == *d );
    BOOST_CHECK( v[9] == *c );
    BOOST_CHECK( 0 == circular->Pop(v) );
    delete d;
    delete c;
}

void BuffersDataFlowTest::testDObj()
{
    Dummy* c = new Dummy(2.0, 1.0, 0.0);
    Dummy  d(5.0, 4.0, 3.0);

    BOOST_REQUIRE_EQUAL( NoData, dataobj->Get(d) );
    BOOST_REQUIRE_EQUAL( d, Dummy(5.0, 4.0, 3.0) );

    dataobj->Set( *c );
    BOOST_REQUIRE_EQUAL( *c, dataobj->Get() );
    int i = 0;
    while ( i != 3.5*dlockfree->MAX_THREADS ) {
        dataobj->Set( *c );
        dataobj->Set( d );
        ++i;
    }
    BOOST_REQUIRE_EQUAL( d , dataobj->Get() );
    BOOST_REQUIRE_EQUAL( d , dataobj->Get() );

    delete c;
}

void BuffersDataFlowTest::testBufMultiThreaded(int number_of_writers, int number_of_readers)
{
    ThreadPool<BufferWriter> writers(number_of_writers, ORO_SCHED_OTHER, 0, 0, "BufferWriter", buffer);
    ThreadPool<BufferReader> readers(number_of_readers, ORO_SCHED_OTHER, 0, 0, "BufferReader", buffer);

    buffer->clear();

    BOOST_REQUIRE( readers.start() );
    BOOST_REQUIRE( writers.start() );
    sleep(5);
    BOOST_REQUIRE( writers.stop() );
    BOOST_REQUIRE( readers.stop() );

    int total_writes = 0, total_dropped = 0, total_reads = 0;
    std::map<FlowStatus, int> total_reads_by_status;
    BOOST_FOREACH(ThreadPool<BufferWriter>::value_type &writer, writers) {
        total_writes += writer.first->writes;
        BOOST_CHECK_GT(writer.first->writes, 0);
    }
    BOOST_FOREACH(ThreadPool<BufferReader>::value_type &reader, readers) {
        total_reads += reader.first->reads;
        BOOST_CHECK_GT(reader.first->reads, 0);
        BOOST_FOREACH(ReadsByStatusMap::value_type &reads_by_status, reader.first->reads_by_status) {
            total_reads_by_status[reads_by_status.first] += reads_by_status.second;
        }
    }

    if (buffer != circular) {
        BOOST_CHECK_EQUAL(total_writes, (total_reads_by_status[NewData] + buffer->size()));
        BOOST_WARN_EQUAL(0, total_dropped);
    } else {
        BOOST_CHECK_GE(total_writes, (total_reads_by_status[NewData] + buffer->size()));
        BOOST_CHECK_EQUAL(0, total_dropped);
    }
}

void BuffersDataFlowTest::testDObjMultiThreaded(int number_of_writers, int number_of_readers)
{
    ThreadPool<DataObjectWriter> writers(number_of_writers, ORO_SCHED_OTHER, 0, 0, "DataObjectWriter", dataobj);
    ThreadPool<DataObjectReader> readers(number_of_readers, ORO_SCHED_OTHER, 0, 0, "DataObjectReader", dataobj);

    BOOST_REQUIRE( readers.start() );
    BOOST_REQUIRE( writers.start() );
    sleep(5);
    BOOST_REQUIRE( writers.stop() );
    BOOST_REQUIRE( readers.stop() );

    int total_writes = 0, total_dropped = 0, total_reads = 0;
    std::map<FlowStatus, int> total_reads_by_status;
    BOOST_FOREACH(ThreadPool<DataObjectWriter>::value_type &writer, writers) {
        total_writes += writer.first->writes;
        BOOST_CHECK_GT(writer.first->writes, 0);
    }
    BOOST_FOREACH(ThreadPool<DataObjectReader>::value_type &reader, readers) {
        total_reads += reader.first->reads;
        BOOST_CHECK_GT(reader.first->reads, 0);
        BOOST_FOREACH(ReadsByStatusMap::value_type &reads_by_status, reader.first->reads_by_status) {
            total_reads_by_status[reads_by_status.first] += reads_by_status.second;
        }
    }

    // BOOST_CHECK_EQUAL(total_writes, total_reads_by_status[NewData]);
    BOOST_CHECK_GE(total_writes, total_reads_by_status[NewData]);
    BOOST_WARN_EQUAL(total_dropped, 0);
}

class BuffersMPoolTest
{
public:

    ThreadInterface* athread;
    ThreadInterface* bthread;

    TsPool<Dummy>* mpool;
    TsPool<std::vector<Dummy> >* vpool;

    BuffersMPoolTest()
    {
        mpool = new TsPool<Dummy>(QS);
        vpool = new TsPool<std::vector<Dummy> >(QS, std::vector<Dummy>(QS) );
    }

    ~BuffersMPoolTest(){
        delete mpool;
        delete vpool;
    }

    template <typename T>
    class Worker : public base::RunnableInterface
    {
    private:
        TsPool<T> *mpool;
        bool stop;
    public:
        int cycles;

        Worker(TsPool<T> *pool) : mpool(pool), stop(false), cycles(0) {}
        bool initialize() {
            stop = false;
            return true;
        }
        void step() {
            while( stop == false ) {
                T *item;
                BOOST_VERIFY( item = mpool->allocate() );
                getThread()->yield();
                if (item) BOOST_VERIFY(mpool->deallocate(item));
                ++cycles;
            }
        }
        bool breakLoop() {
            stop = true;
            return true;
        }
        void finalize() {}
    };

    void testMPoolMultiThreaded(int number_of_workers);
};

void BuffersMPoolTest::testMPoolMultiThreaded(int number_of_workers)
{
    {
        BOOST_CHECK_EQUAL( mpool->size(), QS);
        ThreadPool<BuffersMPoolTest::Worker<Dummy> > workers(number_of_workers, ORO_SCHED_OTHER, 0, 0, "BuffersMPoolWorker", mpool);
        BOOST_REQUIRE( workers.start() );
        sleep(5);
        BOOST_REQUIRE( workers.stop() );
        BOOST_CHECK_EQUAL( mpool->size(), QS);

        int total_cycles = 0;
        BOOST_FOREACH(ThreadPool<BuffersMPoolTest::Worker<Dummy> >::value_type &worker, workers) {
            BOOST_CHECK_GT(worker.first->cycles, 0);
            log(Info) << worker.second->getName() << ": " << worker.first->cycles << " cycles" << endlog();
            total_cycles += worker.first->cycles;
        }
    }
    {
        BOOST_CHECK_EQUAL( vpool->size(), QS);
        ThreadPool<BuffersMPoolTest::Worker<std::vector<Dummy> > > workers(number_of_workers, ORO_SCHED_OTHER, 0, 0, "BuffersVPoolWorker", vpool);
        BOOST_REQUIRE( workers.start() );
        sleep(5);
        BOOST_REQUIRE( workers.stop() );
        BOOST_CHECK_EQUAL( vpool->size(), QS);

        int total_cycles = 0;
        BOOST_FOREACH(ThreadPool<BuffersMPoolTest::Worker<std::vector<Dummy> > >::value_type &worker, workers) {
            BOOST_CHECK_GT(worker.first->cycles, 0);
            log(Info) << worker.second->getName() << ": " << worker.first->cycles << " cycles" << endlog();
            total_cycles += worker.first->cycles;
        }
    }
}


std::ostream& operator<<( std::ostream& os, const Dummy& d )  {
	os << "(" << d.d1 <<","<<d.d2<<","<<d.d3<<")";
	return os;
}
void addOne(Dummy& d)
{
    ++d.d1;
    ++d.d2;
    ++d.d3;
}

void subOne(Dummy& d)
{
    --d.d1;
    --d.d2;
    --d.d3;
}


struct LLFWorker : public RunnableInterface
{
    volatile bool stop;
    typedef ListLockFree<Dummy> T;
    T* mlst;
    int i;
    int appends;
    int erases;
    LLFWorker(T* l ) : stop(false), mlst(l), i(1) {}
    bool initialize() {
        stop = false; i = 1;
        appends = 0; erases = 0;
        return true;
    }
    void step() {
        while (stop == false ) {
            //log(Info) << "Appending, i="<<i<<endlog();
            while ( mlst->append( Dummy(i,i,i) ) ) { ++i; ++appends; }
            //log(Info) << "Erasing, i="<<i<<endlog();
            while ( mlst->erase( Dummy(i-1,i-1,i-1) ) ) { --i; ++erases; }
        }
        //log(Info) << "Stopping, i="<<i<<endlog();
    }

    void finalize() {}

    bool breakLoop() {
        stop = true;
        return true;
    }
};

struct LLFGrower : public RunnableInterface
{
    volatile bool stop;
    typedef ListLockFree<Dummy> T;
    T* mlst;
    int i;
    LLFGrower(T* l ) : stop(false), mlst(l), i(1) {}
    bool initialize() {
        stop = false; i = 1;
        return true;
    }
    void step() {
        // stress growing of list during append/erase.
        while (stop == false && i < 2500 ) {
            // reserve is quite slow.
            mlst->reserve(i);
            ++i;
        }
    }

    void finalize() {}

    bool breakLoop() {
        stop = true;
        return true;
    }
};

/**
 * A Worker Reads and writes the queue.
 */
template<class T>
struct AQWorker : public RunnableInterface
{
    static os::Mutex m;
    bool stop;
    T* mlst;
    int appends;
    int erases;
    Dummy* orig;
    AQWorker(T* l ) : stop(false), mlst(l),appends(0), erases(0) {
        orig = new Dummy( 1,2,3);
    }
    ~AQWorker() {
        delete orig;
    }
    bool initialize() {
        stop = false;
        return true;
    }
    void step() {
        Dummy* d = orig;
        while (stop == false ) {
            //log(Info) << "Appending, i="<<i<<endlog();
            if ( mlst->enqueue( d ) ) { ++appends; }
            //log(Info) << "Erasing, i="<<i<<endlog();
            if ( mlst->dequeue( d ) ) {
                if( *d != *orig) {
                    os::MutexLock lock(m);
                    assert(*d == *orig); // exercise reading returned memory.
                }
                ++erases;
            }
        }
        //log(Info) << "Stopping, i="<<i<<endlog();
    }

    void finalize() {}

    bool breakLoop() {
        stop = true;
        return true;
    }
};

template<class T>
os::Mutex AQWorker<T>::m;

/**
 * A grower stresses the 'overrun' case by flooding
 * the queue.
 */
template<class T>
struct AQGrower : public RunnableInterface
{
    volatile bool stop;
    T* mlst;
    int appends;
    Dummy* orig;
    AQGrower(T* l ) : stop(false), mlst(l), appends(0) {
        orig = new Dummy( 1,2,3);
    }
    ~AQGrower() {
        delete orig;
    }
    bool initialize() {
        stop = false;
        return true;
    }
    void step() {
        // stress full queue
        Dummy* d = orig;
        while (stop == false ) {
            if ( mlst->enqueue(d) ) {
                ++appends;
            }
        }
    }

    void finalize() {}

    bool breakLoop() {
        stop = true;
        return true;
    }
};

/**
 * An Eater stresses the 'underrun' case by emptying
 * the queue.
 */
template<class T>
struct AQEater : public RunnableInterface
{
    volatile bool stop;
    T* mlst;
    int erases;
    AQEater(T* l ) : stop(false), mlst(l), erases(0) {}
    bool initialize() {
        stop = false;
        return true;
    }
    void step() {
        // stress full queue
        Dummy* d;
        while (stop == false ) {
            if ( mlst->dequeue(d) ) {
                //if( *d != *orig)
                //    BOOST_CHECK_EQUAL(*d, *orig); // exercise reading returned memory.
                ++erases;
            }
        }
    }

    void finalize() {}

    bool breakLoop() {
        stop = true;
        return true;
    }
};


BOOST_FIXTURE_TEST_SUITE( BuffersAtomicMWMRQueueTestSuite, BuffersAtomicMWMRQueueTest )

BOOST_AUTO_TEST_CASE( testAtomicMWMRQueue )
{
    /**
     * Single Threaded test for AtomicQueue.
     */
    Dummy* d = new Dummy();
    Dummy* c = d;

    BOOST_REQUIRE_EQUAL( AtomicQueue<Dummy*>::size_type(QS), aqueue->capacity() );
    BOOST_REQUIRE_EQUAL( AtomicQueue<Dummy*>::size_type(0), aqueue->size() );
    BOOST_CHECK( aqueue->isFull() == false );
    BOOST_CHECK( aqueue->isEmpty() == true );
    BOOST_CHECK( aqueue->dequeue(c) == false );
    BOOST_CHECK( c == d );

    for ( int i = 0; i < QS; ++i) {
        BOOST_CHECK( aqueue->enqueue( d ) == true);
        BOOST_REQUIRE_EQUAL( AtomicQueue<Dummy*>::size_type(i+1), aqueue->size() );
    }
    BOOST_REQUIRE_EQUAL( AtomicQueue<Dummy*>::size_type(QS), aqueue->capacity() );
    BOOST_CHECK( aqueue->isFull() == true );
    BOOST_CHECK( aqueue->isEmpty() == false );
    BOOST_CHECK( aqueue->enqueue( d ) == false );
    BOOST_REQUIRE_EQUAL( AtomicQueue<Dummy*>::size_type(QS), aqueue->size() );

    aqueue->dequeue( d );
    BOOST_CHECK( aqueue->isFull() == false );
    BOOST_REQUIRE_EQUAL( AtomicQueue<Dummy*>::size_type(QS-1), aqueue->size() );

    for ( int i = 0; i < QS - 1 ; ++i) {
        BOOST_CHECK( aqueue->dequeue( d ) == true);
        BOOST_REQUIRE_EQUAL( AtomicQueue<Dummy*>::size_type(QS - 2 - i), aqueue->size() );
    }
    BOOST_CHECK( aqueue->isFull() == false );
    BOOST_CHECK( aqueue->isEmpty() == true );

    delete d;
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE( BuffersAtomicMWSRQueueTestSuite, BuffersAtomicMWSRQueueTest )

BOOST_AUTO_TEST_CASE( testAtomicMWSRQueue )
{
    /**
     * Single Threaded test for AtomicQueue.
     */
    Dummy* d = new Dummy();
    Dummy* c = d;

    BOOST_REQUIRE_EQUAL( AtomicQueue<Dummy*>::size_type(QS), aqueue->capacity() );
    BOOST_REQUIRE_EQUAL( AtomicQueue<Dummy*>::size_type(0), aqueue->size() );
    BOOST_CHECK( aqueue->isFull() == false );
    BOOST_CHECK( aqueue->isEmpty() == true );
    BOOST_CHECK( aqueue->dequeue(c) == false );
    BOOST_CHECK( c == d );

    for ( int i = 0; i < QS; ++i) {
        BOOST_CHECK( aqueue->enqueue( d ) == true);
        BOOST_REQUIRE_EQUAL( AtomicQueue<Dummy*>::size_type(i+1), aqueue->size() );
        BOOST_CHECK( d );
    }
    BOOST_REQUIRE_EQUAL( AtomicQueue<Dummy*>::size_type(QS), aqueue->capacity() );
    BOOST_CHECK( aqueue->isFull() == true );
    BOOST_CHECK( aqueue->isEmpty() == false );
    BOOST_CHECK( aqueue->enqueue( d ) == false );
    BOOST_REQUIRE_EQUAL( AtomicQueue<Dummy*>::size_type(QS), aqueue->size() );
    d = 0;
    aqueue->dequeue( d );
    BOOST_CHECK( d ); // not null
    BOOST_CHECK( aqueue->isFull() == false );
    BOOST_REQUIRE_EQUAL( AtomicQueue<Dummy*>::size_type(QS-1), aqueue->size() );

    for ( int i = 0; i < QS - 1 ; ++i) {
        BOOST_CHECK( aqueue->dequeue( d ) == true);
        BOOST_REQUIRE_EQUAL( AtomicQueue<Dummy*>::size_type(QS - 2 - i), aqueue->size() );
        BOOST_CHECK( d );
    }
    BOOST_CHECK( aqueue->isFull() == false );
    BOOST_CHECK( aqueue->isEmpty() == true );
    BOOST_CHECK( aqueue->dequeue(d) == false );
    BOOST_CHECK( d ); // not null

    delete d;
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE( BuffersDataFlowTestSuite, BuffersDataFlowTest )

BOOST_AUTO_TEST_CASE( testBufLockFree )
{
    buffer = lockfree;
    circular = clockfree;
    testBuf();
    testCirc();
}

BOOST_AUTO_TEST_CASE( testBufLocked )
{
    buffer = locked;
    circular = clocked;
    testBuf();
    testCirc();
}

BOOST_AUTO_TEST_CASE( testBufUnsync )
{
    buffer = unsync;
    circular = cunsync;
    testBuf();
    testCirc();
}

BOOST_AUTO_TEST_CASE( testDObjLockFree )
{
    dataobj = dlockfree;
    testDObj();
}

BOOST_AUTO_TEST_CASE( testDObjLocked )
{
    dataobj = dlocked;
    testDObj();
}

BOOST_AUTO_TEST_CASE( testDObjUnSync )
{
    dataobj = dunsync;
    testDObj();
}

BOOST_AUTO_TEST_CASE( testBufLockFree4Writers1Reader )
{
    buffer = lockfree;
    testBufMultiThreaded(4, 1);
    buffer = circular = clockfree;
    testBufMultiThreaded(4, 1);
}

BOOST_AUTO_TEST_CASE( testBufLockFree4Writers4Readers )
{
    buffer = new BufferLockFree<Dummy>(QS, Dummy(), BufferBase::Options().circular(false).multiple_readers(true));
    testBufMultiThreaded(4, 4);
    delete buffer;

    buffer = circular = new BufferLockFree<Dummy>(QS, Dummy(), BufferBase::Options().circular(true).multiple_readers(true));
    testBufMultiThreaded(4, 4);
    delete buffer;
}

BOOST_AUTO_TEST_CASE( testBufLocked4Writers4Readers )
{
    buffer = locked;
    testBufMultiThreaded(4, 4);
    buffer = circular = clocked;
    testBufMultiThreaded(4, 4);
}

BOOST_AUTO_TEST_CASE( testDObjLockFreeSingleWriter4Readers )
{
    dataobj = new DataObjectLockFree<Dummy>(Dummy(), /* max_threads = */ 5);
    testDObjMultiThreaded(1, 4);
    delete dataobj;
}

BOOST_AUTO_TEST_CASE( testDObjLockedSingleWriter4Readers )
{
    dataobj = dlocked;
    testDObjMultiThreaded(1, 4);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_FIXTURE_TEST_SUITE( BuffersMPoolTestSuite, BuffersMPoolTest )

BOOST_AUTO_TEST_CASE( testMemoryPool )
{
    // Test initial conditions.
    TsPool<Dummy>::size_type sz = QS;
    // Capacity test
    BOOST_REQUIRE_EQUAL( sz, mpool->capacity() );
    BOOST_REQUIRE_EQUAL( sz, vpool->capacity() );
    BOOST_CHECK_EQUAL( sz, mpool->size());
    BOOST_CHECK_EQUAL( sz, vpool->size());

    // test default initialiser:
    for (TsPool<Dummy>::size_type i = 0; i <3*sz; ++i ) {
        // MemoryPool:
        std::vector<Dummy>* v = vpool->allocate();
        BOOST_CHECK_EQUAL( sz - 1, vpool->size());
        std::vector<Dummy>::size_type szv = QS;
        BOOST_REQUIRE_EQUAL( szv, v->size() );
        BOOST_REQUIRE_EQUAL( szv, v->capacity() );
        BOOST_CHECK(vpool->deallocate( v ));
        BOOST_CHECK_EQUAL( sz, vpool->size());
    }
    BOOST_CHECK_EQUAL( vpool->size(), QS);

    // test Allocation.
    std::vector<Dummy*> mpv;
    // MemoryPool:
    for (TsPool<Dummy>::size_type i = 0; i <sz; ++i ) {
        mpv.push_back( mpool->allocate() );
        BOOST_CHECK_EQUAL( sz - i - 1, mpool->size());
        BOOST_CHECK( mpv.back() );
        BOOST_REQUIRE_EQUAL( sz, mpool->capacity() );
    }
    BOOST_CHECK_EQUAL( mpool->size(), 0);
    BOOST_CHECK_EQUAL( mpool->allocate(), (Dummy*)0 );
    for (TsPool<Dummy>::size_type i = 0; i <sz; ++i ) {
        BOOST_CHECK_EQUAL( i , mpool->size());
        BOOST_CHECK(mpool->deallocate( mpv.front() ));
        BOOST_CHECK_EQUAL( i + 1, mpool->size());
        mpv.erase( mpv.begin() );
        BOOST_REQUIRE_EQUAL( sz, mpool->capacity() );
    }
    BOOST_CHECK_EQUAL( mpv.size(), 0 );
    BOOST_CHECK_EQUAL( mpool->size(), QS);
}

BOOST_AUTO_TEST_CASE( testMemoryPoolMultiThreaded )
{
    Logger::In in("testMemoryPoolMultiThreaded");
    testMPoolMultiThreaded(QS);
}

#if 0
BOOST_AUTO_TEST_CASE( testSortedList )
{
    // 7 elements.
    mslist->reserve(7);
    BOOST_CHECK( mslist->empty() );

    // empty list has no keys.
    BOOST_CHECK( mslist->hasKey(Dummy()) == false );

    // empty list fails to erase key.
    BOOST_CHECK( mslist->erase(Dummy()) == false );

    // insert element once
    BOOST_CHECK( mslist->insert(Dummy(1,2,1)) == true );
    BOOST_CHECK( mslist->hasKey(Dummy(1,2,1)) == true );

    BOOST_CHECK( mslist->insert(Dummy(1,2,1)) == false );
    BOOST_CHECK( mslist->hasKey(Dummy(1,2,1)) == true );

    // erase element once
    BOOST_CHECK( mslist->erase(Dummy(1,2,1)) == true );
    BOOST_CHECK( mslist->hasKey(Dummy(1,2,1)) == false );
    BOOST_CHECK( mslist->erase(Dummy(1,2,1)) == false );
    BOOST_CHECK( mslist->hasKey(Dummy(1,2,1)) == false );

    BOOST_CHECK( mslist->insert(Dummy(1,2,1)) == true );
    BOOST_CHECK( mslist->insert(Dummy(1,2,2)) == true );
    BOOST_CHECK( mslist->insert(Dummy(1,2,3)) == true );
    BOOST_CHECK( mslist->insert(Dummy(1,2,4)) == true );
    BOOST_CHECK( mslist->insert(Dummy(1,2,5)) == true );
    BOOST_CHECK( mslist->insert(Dummy(1,2,6)) == true );
    BOOST_CHECK( mslist->insert(Dummy(1,2,7)) == true );

    BOOST_CHECK( mslist->hasKey(Dummy(1,2,4)) == true );
    BOOST_CHECK( mslist->hasKey(Dummy(1,2,7)) == true );

    BOOST_CHECK( mslist->erase(Dummy(1,2,7)) == true );
    BOOST_CHECK( mslist->hasKey(Dummy(1,2,7)) == false );

    BOOST_CHECK( mslist->erase(Dummy(1,2,4)) == true );
    BOOST_CHECK( mslist->hasKey(Dummy(1,2,4)) == false );

    mslist->applyOnData( &addOne );
    BOOST_CHECK( mslist->hasKey(Dummy(2,3,2)) == true );
    BOOST_CHECK( mslist->hasKey(Dummy(2,3,3)) == true );
    BOOST_CHECK( mslist->hasKey(Dummy(2,3,4)) == true );
    BOOST_CHECK( mslist->hasKey(Dummy(2,3,6)) == true );
    BOOST_CHECK( mslist->hasKey(Dummy(2,3,7)) == true );

    mslist->applyOnData( &subOne );
    BOOST_CHECK( mslist->hasKey(Dummy(1,2,1)) == true );
    BOOST_CHECK( mslist->hasKey(Dummy(1,2,2)) == true );
    BOOST_CHECK( mslist->hasKey(Dummy(1,2,3)) == true );
    BOOST_CHECK( mslist->hasKey(Dummy(1,2,5)) == true );
    BOOST_CHECK( mslist->hasKey(Dummy(1,2,6)) == true );

    BOOST_CHECK( mslist->erase(Dummy(1,2,1)) == true );
    BOOST_CHECK( mslist->erase(Dummy(1,2,6)) == true );
    BOOST_CHECK( mslist->erase(Dummy(1,2,5)) == true );
    BOOST_CHECK( mslist->erase(Dummy(1,2,2)) == true );
    BOOST_CHECK( mslist->erase(Dummy(1,2,3)) == true );

    BOOST_CHECK( mslist->empty() );
}
#endif

#ifdef OROPKG_OS_GNULINUX

BOOST_AUTO_TEST_SUITE_END()
BOOST_FIXTURE_TEST_SUITE( BuffersStressLockFreeTestSuite, BuffersAtomicMWMRQueueTest )

BOOST_AUTO_TEST_CASE( testListLockFree )
{
    LLFWorker* aworker = new LLFWorker( listlockfree );
    LLFWorker* bworker = new LLFWorker( listlockfree );
    LLFWorker* cworker = new LLFWorker( listlockfree );
    LLFGrower* grower = new LLFGrower( listlockfree );

    {
        boost::scoped_ptr<Activity> athread( new Activity(ORO_SCHED_OTHER, 0, 0, aworker, "ActivityA" ));
        boost::scoped_ptr<Activity> bthread( new Activity(ORO_SCHED_OTHER, 0, 0, bworker, "ActivityB" ));
        boost::scoped_ptr<Activity> cthread( new Activity(ORO_SCHED_OTHER, 0, 0, cworker, "ActivityC" ));
        boost::scoped_ptr<Activity> gthread( new Activity(ORO_SCHED_OTHER, 0, 0, grower, "ActivityG" ));

        athread->start();
        bthread->start();
        cthread->start();

        sleep(5);
        gthread->start();
        sleep(10);
        gthread->stop();
        sleep(5);

        athread->stop();
        bthread->stop();
        cthread->stop();
    }

#if 0
    log(Info) << "Athread appends: " << aworker->appends<<endlog();
    log(Info) << "Athread erases: " << aworker->erases<<endlog();
    log(Info) << "Bthread appends: " << bworker->appends<<endlog();
    log(Info) << "Bthread erases: " << bworker->erases<<endlog();
    log(Info) << "Cthread appends: " << cworker->appends<<endlog();
    log(Info) << "Cthread erases: " << cworker->erases<<endlog();
    log(Info) << "List capacity: "<< listlockfree->capacity()<<endlog();
    log(Info) << "List size: "<< listlockfree->size()<<endlog();
//     while( listlockfree->empty() == false ) {
//         Dummy d =  listlockfree->back();
//         //log(Info) << "Left: "<< d <<endlog();
//         BOOST_CHECK( listlockfree->erase( d ) );
//     }
#endif

    BOOST_CHECK( aworker->appends == aworker->erases );
    BOOST_CHECK( bworker->appends == bworker->erases );
    BOOST_CHECK( cworker->appends == cworker->erases );

    delete aworker;
    delete bworker;
    delete cworker;
    delete grower;
}
#endif

#ifdef OROPKG_OS_GNULINUX
BOOST_AUTO_TEST_CASE( testAtomicQueue )
{
    Logger::In in("testAtomicQueue");

    MWMRQueueType* qt = new MWMRQueueType(QS);
    AQWorker<MWMRQueueType>* aworker = new AQWorker<MWMRQueueType>( qt );
    AQWorker<MWMRQueueType>* bworker = new AQWorker<MWMRQueueType>( qt );
    AQWorker<MWMRQueueType>* cworker = new AQWorker<MWMRQueueType>( qt );
    AQGrower<MWMRQueueType>* grower = new AQGrower<MWMRQueueType>( qt );
    AQEater<MWMRQueueType>* eater = new AQEater<MWMRQueueType>( qt );

    {
        boost::scoped_ptr<Activity> athread( new Activity(20, aworker, "ActivityA" ));
        boost::scoped_ptr<Activity> bthread( new Activity(20, bworker, "ActivityB" ));
        boost::scoped_ptr<Activity> cthread( new Activity(20, cworker, "ActivityC" ));
        boost::scoped_ptr<Activity> gthread( new Activity(20, grower, "ActivityG"));
        boost::scoped_ptr<Activity> ethread( new Activity(20, eater, "ActivityE"));

        // avoid system lock-ups
        athread->thread()->setScheduler(ORO_SCHED_OTHER);
        bthread->thread()->setScheduler(ORO_SCHED_OTHER);
        cthread->thread()->setScheduler(ORO_SCHED_OTHER);
        gthread->thread()->setScheduler(ORO_SCHED_OTHER);

        log(Info) <<"Stressing multi-read/multi-write..." <<endlog();
        athread->start();
        bthread->start();
        cthread->start();
        sleep(5);
        log(Info) <<"Stressing multi-read/multi-write...on full buffer" <<endlog();
        gthread->start(); // stress full bufs
        sleep(5);
        gthread->stop();
        log(Info) <<"Stressing multi-read/multi-write...on empty buffer" <<endlog();
        ethread->start(); // stress empty bufs
        sleep(5);
        athread->stop();
        bthread->stop();
        cthread->stop();
        gthread->start(); // stress single-reader single-writer
        log(Info) <<"Stressing read&write..." <<endlog();
        sleep(5);
        gthread->stop();
        ethread->stop();
    }

    log(Info) <<endlog()
         << "Total appends: " << aworker->appends + bworker->appends + cworker->appends+ grower->appends<<endlog();
    log(Info) << "Total erases : " << aworker->erases + bworker->erases+ cworker->erases + qt->size() + eater->erases <<endlog();
    if (aworker->appends + bworker->appends + cworker->appends+ grower->appends != aworker->erases + bworker->erases+ cworker->erases + int(qt->size()) + eater->erases) {
        log(Info) << "Mismatch detected !" <<endlog();
    }
    int i = 0; // left-over count
    Dummy* d = 0;
    BOOST_CHECK( qt->size() <= QS );
    while( qt->size() != 0 ) {
        BOOST_CHECK( qt->dequeue(d) == true);
        BOOST_CHECK( d );
        i++;
        if ( i > QS ) {
            BOOST_CHECK( i <= QS); // avoid infinite loop.
            break;
        }
    }
    log(Info) << "Left in Queue: "<< i <<endlog();
    BOOST_CHECK( qt->dequeue(d) == false );
    BOOST_CHECK( qt->dequeue(d) == false );
    BOOST_CHECK( qt->isEmpty() );
    BOOST_CHECK_EQUAL( qt->size(), 0 );

    // assert: sum queues == sum dequeues
    BOOST_CHECK_EQUAL( aworker->appends + bworker->appends + cworker->appends + grower->appends,
                       aworker->erases + bworker->erases + cworker->erases + i + eater->erases );
    delete aworker;
    delete bworker;
    delete cworker;
    delete grower;
    delete eater;
}

BOOST_AUTO_TEST_CASE( testAtomicMWSRQueue )
{
    Logger::In in("testAtomicMWSRQueue");

    MWSRQueueType* qt = new MWSRQueueType(QS);
    AQGrower<MWSRQueueType>* aworker = new AQGrower<MWSRQueueType>( qt );
    AQGrower<MWSRQueueType>* bworker = new AQGrower<MWSRQueueType>( qt );
    AQGrower<MWSRQueueType>* cworker = new AQGrower<MWSRQueueType>( qt );
    AQGrower<MWSRQueueType>* grower = new AQGrower<MWSRQueueType>( qt );
    AQEater<MWSRQueueType>* eater = new AQEater<MWSRQueueType>( qt );

    {
        boost::scoped_ptr<Activity> athread( new Activity(20, aworker, "ActivityA" ));
        boost::scoped_ptr<Activity> bthread( new Activity(20, bworker, "ActivityB" ));
        boost::scoped_ptr<Activity> cthread( new Activity(20, cworker, "ActivityC" ));
        boost::scoped_ptr<Activity> gthread( new Activity(20, grower, "ActivityG"));
        boost::scoped_ptr<Activity> ethread( new Activity(20, eater, "ActivityE"));

        // avoid system lock-ups
        athread->thread()->setScheduler(ORO_SCHED_OTHER);
        bthread->thread()->setScheduler(ORO_SCHED_OTHER);
        cthread->thread()->setScheduler(ORO_SCHED_OTHER);
        gthread->thread()->setScheduler(ORO_SCHED_OTHER);
        ethread->thread()->setScheduler(ORO_SCHED_OTHER);

        log(Info) <<"Stressing multi-write/single-read..." <<endlog();
        athread->start();
        bthread->start();
        cthread->start();
        gthread->start();
        ethread->start();
        sleep(5);
        athread->stop();
        bthread->stop();
        cthread->stop();
        log(Info) <<"Stressing single-write/single-read..." <<endlog();
        sleep(5);
        gthread->stop();
        ethread->stop();
    }

    log(Info) << nlog()
         << "Total appends: " << aworker->appends + bworker->appends + cworker->appends+ grower->appends<<endlog();
    log(Info) << "Total erases : " << eater->erases <<endlog();
    if (aworker->appends + bworker->appends + cworker->appends+ grower->appends != int(qt->size()) + eater->erases) {
        log(Info) << "Mismatch detected !" <<endlog();
    }
    int i = 0; // left-over count
    Dummy* d = 0;
    BOOST_CHECK( qt->size() <= QS );
    while( qt->size() != 0 ) {
        BOOST_CHECK( qt->dequeue(d) == true);
        BOOST_CHECK( d );
        i++;
        if ( i > QS ) {
            BOOST_CHECK( i <= QS); // avoid infinite loop.
            break;
        }
    }
    log(Info) << "Left in Queue: "<< i <<endlog();
    BOOST_CHECK( qt->dequeue(d) == false );
    BOOST_CHECK( qt->dequeue(d) == false );
    BOOST_CHECK( qt->isEmpty() );
    BOOST_CHECK_EQUAL( qt->size(), 0 );

    // assert: sum queues == sum dequeues
    BOOST_CHECK_EQUAL( aworker->appends + bworker->appends + cworker->appends + grower->appends,
                       i + eater->erases );
    delete aworker;
    delete bworker;
    delete cworker;
    delete grower;
    delete eater;
    delete qt;
}
#endif
BOOST_AUTO_TEST_SUITE_END()
