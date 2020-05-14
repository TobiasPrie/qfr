#ifndef QFR_SHOR_H
#define QFR_SHOR_H

#include <QuantumComputation.hpp>

namespace qc {
	class Shor : public QuantumComputation {
	private:
		int iModN(int value);

	protected:
		void setup(QuantumComputation& qc);

		void oracle(QuantumComputation& qc);

		void Ua(QuantumComputation& qc, std::vector<Control> controls, int value, int startQB);

		void ADD(QuantumComputation& qc, std::vector<Control> controls, int startQB, long value, bool invert = false);

		void ADDmodN(QuantumComputation& qc, std::vector<Control> controls, long value, int startQB);

		void iADDmodN(QuantumComputation& qc, std::vector<Control> controlsOut, long value, int startQB);

		void CMULTmodN(QuantumComputation& qc, std::vector<Control> controls, int value, int startQB);

		void iCMULTmodN(QuantumComputation& qc, std::vector<Control> controls, int value, int startQB);

		void CSWAP(QuantumComputation& qc, std::vector<Control> controls, int qubitA, int qubitB);

		void QFT(QuantumComputation& qc, int startQB, int nInputQB);

		void iQFT(QuantumComputation& qc, int startQB, int nInputQB);

		void full_Shor(QuantumComputation& qc);

	public:
		unsigned long	   N;
		unsigned long      a;
		unsigned long	   size = -1;

		explicit Shor(unsigned long n, unsigned long a);

		~Shor() override = default;

		std::ostream& printStatistics(std::ostream& os = std::cout) override;

	};
}
#endif //QFR_SHOR_H
