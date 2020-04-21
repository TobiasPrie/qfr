#include "algorithms/Shor.hpp"
#include "gtest/gtest.h"

class Shor : public testing::TestWithParam<int> {
protected:
	void TearDown() override {}
	void SetUp() override {}
};

INSTANTIATE_TEST_SUITE_P(Shor, Shor,
	testing::Values(5ull, 10ull, 50ull),
	[](const testing::TestParamInfo<Shor::ParamType>& info) {

		// Generate names for test cases
		int a = info.param;
		std::stringstream ss{};
		ss << a << "_a Value";
		return ss.str();
	});

TEST_P(Shor, FunctionTest) {
	std::bitset<64> a = GetParam();
	auto dd = std::make_unique<dd::Package>();
	std::unique_ptr<qc::Shor> qc;
	dd::Edge e{};

	// Create the QuantumCircuite with the hidden integer
	ASSERT_NO_THROW({ qc = std::make_unique<qc::Shor>(200, a); });
	ASSERT_NO_THROW({ e = qc->buildFunctionality(dd); });

	dd::Edge r = dd->multiply(e, dd->makeZeroState(qc->size));
	std::string hIntPath = std::string(qc->size, '0');

	dd::Edge result = qc->simulate(r, dd);

	dd->printVector(result);
}
