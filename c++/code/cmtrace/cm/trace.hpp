/***********************************************************
 * @Filename    : trace.hpp
 * @Copyright   : Mockey Chen 2006 (C)
 * @Author      : Mockey chen<mockey.chen@gmail.com>
 * @Date        : 2006-02-04
 * @Description : Debug trace utility
 *                Free to use, modify, redistribute.
 *
 * @Revision:
 *  v1.00   2006-02-04 Mockey Chen
 *          Initialize.
 *
 ***********************************************************/

#include <string>
#include <fstream>
#include <iomanip>

#define CM_TRACE_FILE(trace_file)   cm::Trace::LogToFile(trace_file)
#define CM_TRACE(func_name)         cm::Trace __CM_TRACE__(func_name)

namespace	cm
{
    class Trace
    {
    public:
        explicit Trace(const std::string& func_name)
            : func_name_("")
        {
            func_name_ = func_name;
            Record(depth_, depth_*2, func_name_, "Enter");
            ++depth_;
        }

        ~Trace()
        {
            --depth_;
            Record(depth_, depth_*2, func_name_, "Leave");
        }

        /// special the global log file.
        void static LogToFile(const std::string& trace_file)
        {
            trace_file_ = trace_file;
        }

    private:
        void Record(int depth,
            int align,
            const std::string& func_name,
            const std::string& action)
        {
            std::ofstream ofs(trace_file_.c_str(),
                std::ios::out|std::ios::app);

            if (ofs)
            {
                ofs << std::setw(align) << std::setfill(' ')
                    << '(' << depth << ')' << "[BT]:"
                    << action
                    << " function <"
                    << func_name
                    << ">"
                    << std::endl;

                ofs.close();
            }
        }

    private:
        /// current traced function name
        std::string         func_name_;

        /// the debug trace filename
        static std::string  trace_file_;

        /// function call stack depth
        static int          depth_;
    };

    std::string Trace::trace_file_ = "";
    int    Trace::depth_           = 0;
} // end namespace cm
