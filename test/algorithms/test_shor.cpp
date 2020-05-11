#include "algorithms/Shor.hpp"
#include "gtest/gtest.h"

class Shor : public testing::TestWithParam<int> {
protected:
	void TearDown() override {}
	void SetUp() override {}
};

INSTANTIATE_TEST_SUITE_P(Shor, Shor,
	testing::Values(2, 3, 4),
	[](const testing::TestParamInfo<Shor::ParamType>& info) {

		// Generate names for test cases
		int a = info.param;
		std::stringstream ss{};
		ss << a << "_a_Value";
		return ss.str();
	});

TEST_P(Shor, FunctionTest) {
	std::bitset<64> a = GetParam();
	auto dd = std::make_unique<dd::Package>();
	std::unique_ptr<qc::Shor> qc;
	dd::Edge e{};

	// Create the QuantumCircuite with the hidden integer
	ASSERT_NO_THROW({ qc = std::make_unique<qc::Shor>(6ull, a.to_ulong()); });
	ASSERT_NO_THROW({ e = qc->buildFunctionality(dd); });

	dd::Edge r = dd->multiply(e, dd->makeZeroState(qc->getNqubits()));

	dd::Edge result = qc->simulate(r, dd);

	std::string testA = std::string(qc->getNqubits(), '0');

	//qc->print(std::cout);
	dd->printVector(r);
	EXPECT_EQ(true, false);
}
