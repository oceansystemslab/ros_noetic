/*
 * Copyright (c) 2008, Willow Garage, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Willow Garage, Inc. nor the names of its
 *       contributors may be used to endorse or promote products derived from
 *       this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "ros/console.h"

#include <gtest/gtest.h>

#include <boost/thread.hpp>

#include "log4cxx/appenderskeleton.h"
#include "log4cxx/spi/loggingevent.h"
#ifdef _MSC_VER
  // Have to be able to encode wchar LogStrings on windows.
  #include "log4cxx/helpers/transcoder.h"
#endif

#include <vector>

class TestAppender : public log4cxx::AppenderSkeleton
{
public:
  struct Info
  {
    log4cxx::LevelPtr level_;
    std::string message_;
    std::string logger_name_;
  };

  typedef std::vector<Info> V_Info;

  V_Info info_;

protected:
  virtual void append(const log4cxx::spi::LoggingEventPtr& event, log4cxx::helpers::Pool&)
  {
    Info info;
    info.level_ = event->getLevel();
#ifdef _MSC_VER
    LOG4CXX_ENCODE_CHAR(msgstr, event->getMessage());  // has to handle LogString with wchar types.
    info.message_ = msgstr;  // msgstr gets instantiated inside the LOG4CXX_ENCODE_CHAR macro

    LOG4CXX_ENCODE_CHAR(loggerstr, event->getLoggerName());  // has to handle LogString with wchar types.
    info.logger_name_ = loggerstr;  // loggerstr gets instantiated inside the LOG4CXX_ENCODE_CHAR macro
#else
    info.message_ = event->getMessage();
    info.logger_name_ = event->getLoggerName();
#endif

    info_.push_back( info );
  }

  virtual void close()
  {
  }
  virtual bool requiresLayout() const
  {
    return false;
  }
};
LOG4CXX_PTR_DEF(TestAppender);

void threadFunc(boost::barrier* b)
{
  b->wait();
  ROS_INFO("Hello");
}

// Ensure all threaded calls go out
TEST(Rosconsole, threadedCalls)
{
  log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger(ROSCONSOLE_DEFAULT_NAME);

  auto appender = TestAppenderPtr(new TestAppender);
  logger->addAppender( appender );

  boost::thread_group tg;
  boost::barrier b(10);
  for (uint32_t i = 0; i < 10; ++i)
  {
    tg.create_thread(boost::bind(threadFunc, &b));
  }
  tg.join_all();

  ASSERT_EQ(appender->info_.size(), 10ULL);

  logger->removeAppender(appender);
}

int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  ros::Time::init();

  return RUN_ALL_TESTS();
}
