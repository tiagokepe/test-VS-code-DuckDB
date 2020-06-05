#include "duckdb.hpp"
#include "duckdb/common/types/string_type.hpp"
#include <iostream>

#include "re2/re2.h"
#include "duckdb/common/helper.hpp"
#include "test/include/expression_helper.hpp"
#include "test/include/test_helpers.hpp"
#include "test/common/test_gzip_stream_header.hpp"


#include "duckdb/planner/expression/bound_function_expression.hpp"
#include "duckdb/execution/expression_executor.hpp"
#include "duckdb/optimizer/rule/arithmetic_simplification.hpp"

#include "duckdb/main/prepared_statement.hpp"
#include "duckdb/common/types/vector.hpp"
#include "duckdb/common/vector_operations/unary_executor.hpp"
#include <typeinfo>
#include <type_traits>

//#include "udf_utils.hpp"

using namespace duckdb;
using namespace std;

int udf_function(int i) {
	return i;
}

int udf_fucntion(Vector &result, int i) {
	// Vector &result allows heap allocation inside the UDF function, e.g.: StringVector::AddString(result, ...)
	return i;
}

template<typename Ret, typename Arg, idx_t COUNT=1> scalar_function_t CreateUnaryScalarFunction(Ret (*func)(Arg)) {
    std::function<Ret(Arg)> f_func = [&](Arg arg0) {
    	return func(arg0);
    };

    std::function<void(DataChunk &, ExpressionState &, Vector &)> scalar_func =
    		[&] (DataChunk &args, ExpressionState &state, Vector &result) {
					UnaryExecutor::Execute<Arg, Ret>(
							args.data[0],
							result,
							COUNT,
							f_func
					);
    };

    return (scalar_function_t &)scalar_func;
}

namespace duckdb {
enum class FunctionType  {UNARY_FUNCTION, BINARY_FUNCTION, TERNARY_FUNCTION};
}

template<class Ret, class ...Args> void duckdb_create_function(
		string fName,
		vector<duckdb::SQLType>	 arguments,
	    duckdb::SQLType          returnType,
	    duckdb::FunctionType     funType, // [UnaryFunction | BinaryFunction]
	    Ret (*xFun)(Args...),
	    bool                     useResultVector,
	    unique_ptr<FunctionData> bindInfo // this can be similar to the fifth parameter (void *pApp) of the create_function in SQLite3)
		)
		{

	const std::size_t argc = sizeof...(Args);
	cout << argc << endl;

	switch(argc) {
		case 1:
			CreateUnaryScalarFunction<Ret, Args...>(xFun);
			break;
		case 2:
			break;
		case 3:
			break;
		default:
			break;
	}
}

template<class Ret, class Arg> std::function<Ret(Arg)> Test(Ret (*func)(Arg)) {
    std::function<Ret(Arg)> ret_func = [&](Arg arg0) {
    	return func(arg0);
    };
    return ret_func;
}

template<class Ret, class ...Args> std::function<Ret(Args...)> create_udf_function(Ret (*xFun)(Args...)) {
    std::function<Ret(Args...)> ret_func = [&](Args... args) {
    	return xFun(args...);
    };
    return ret_func;
}

int main() {
	idx_t t;
	vector<duckdb::SQLType> arguments = {duckdb::SQLType::INTEGER};
	duckdb::SQLType 		  retType;
	FunctionType          funType = FunctionType::UNARY_FUNCTION;
	switch(funType) {
	case FunctionType::UNARY_FUNCTION:
		assert(arguments.size() == 1);
		break;
	case FunctionType::BINARY_FUNCTION:
		break;
	}

//	auto f = Test<int, int>(&udf_function);
//	auto i = f(5);
//	cout << i << endl;
//
//	auto f_t = create_udf_function<int, int>(&udf_function);
//	cout << f_t(99) << endl;

//	Kepe<int, float>();
	// TODO decidir que tipo será arguments and resultType

//	vector<duckdb::TypeId> argumentsType;
//	auto udf = CreateUnaryScalarFunction<int, int>(argumentsType.size());

//
//
//	unique_ptr<QueryResult> res;
//	DuckDB db(nullptr);
//	Connection con(db);
//	res = con.Query("SELECT kepe_repeat('aaaaaaaaaa',100)");
//	res->Print();
}
