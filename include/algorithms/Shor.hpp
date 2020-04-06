#ifndef QFR_SHOR_H
#define QFR_SHOR_H

#include <QuantumComputation.hpp>

namespace qc {
	class Shor : public QuantumComputation {
	protected:
		void setup(QuantumComputation& qc);

		void oracle(QuantumComputation& qc);

		void Ua(QuantumComputation& qc);

		void iQFT(QuantumComputation& qc);

		void full_Shor(QuantumComputation& qc);

	public:
		unsigned long	   n;
		unsigned long      a;
		unsigned long	   size = -1;

		explicit Shor(unsigned long n, unsigned long a);

		~Shor() override = default;

		std::ostream& printStatistics(std::ostream& os = std::cout) override;

	};
}
#endif //QFR_SHOR_H
