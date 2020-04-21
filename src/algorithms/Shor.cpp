#include <cmath>
#include "algorithms/Shor.hpp"

// BernsteinVazirani without Entanglement
namespace qc {
	/***
	 * Private Methods
	 ***/
	void Shor::setup(QuantumComputation& qc) {
		
	}

	void Shor::oracle(QuantumComputation& qc) {
		// TODO: Implement Oracle
	}

	void Shor::Ua(QuantumComputation& qc) {
		// TODO: Implement Ua
	}

	// Adds a classical number to a QF-transformed number. Size is defined as N+1.
	// startQB: The first qubit in the Adder
	// number: The classical number
	void Shor::pADD(QuantumComputation& qc, int startQB, unsigned long number) 
	{
		// Loop traversing from qubit x to x+n over n qubits, where n is the bitsize of N. 
		// The last additional qubit is to prevent overflow after the QFT. 
		for (unsigned short i = startQB; i < startQB + size; ++i) {
			std::bitset<64> aBitSet(number);

			// Precalculating the Phaseshift
			double phaseShift = 0;
			for (unsigned short j = 0; j <= i - startQB; ++j) {
				phaseShift += aBitSet[j] / std::pow(2, i - startQB - j);
			}
			phaseShift *= static_cast<fp>(PI);

			// Add the gate with the precalculated phaseShift = Ai
			emplace_back<StandardOperation>(nqubits, i, RZ, phaseShift);
		}
	}

	void Shor::pADDmodN(QuantumComputation& qc) 
	{
		// TODO: Implement pADDmodN gate
	}

	// Quantum Fourier Transform block
	// startQB: The first qubit in the QFT
	// qftSize: The number of qubits in the QFT
	void Shor::QFT(QuantumComputation& qc, int startQB, int qftSize) {
		for (unsigned short i = startQB; i < startQB + qftSize; ++i) {
			emplace_back<StandardOperation>(nqubits, i, H);
			for (unsigned short j = 1; j < qftSize - (i - startQB); ++j) {
				long double powerOfTwo = std::pow(2.L, j);
				auto lambda = static_cast<fp>(PI / powerOfTwo);
				if (j == 1) {
					emplace_back<StandardOperation>(nqubits, Control(i + 1), i, S);
				}
				else if (j == 2) {
					emplace_back<StandardOperation>(nqubits, Control(i + 2), i, T);
				}
				else {
					emplace_back<StandardOperation>(nqubits, Control(i + j), i, RZ, lambda);
				}
			}
		}
	}

	// Inverse Quantum Fourier Transform block
	// startQB: The first qubit in the iQFT
	// qftSize: The number of qubits in the iQFT
	void Shor::iQFT(QuantumComputation& qc, int startQB, int qftSize) {
		for (unsigned short i = startQB + qftSize - 1; i >= startQB; --i) {
			for (unsigned short j = 1; j < qftSize - (i - startQB); ++j) {
				long double powerOfTwo = std::pow(2.L, j);
				auto lambda = static_cast<fp>(PI / powerOfTwo);
				if (j == 1) {
					emplace_back<StandardOperation>(nqubits, Control(i + 1), i, Sdag);
				}
				else if (j == 2) {
					emplace_back<StandardOperation>(nqubits, Control(i + 2), i, Tdag);
				}
				else {
					emplace_back<StandardOperation>(nqubits, Control(i + j), i, RZ, -lambda);
				}
			}
			emplace_back<StandardOperation>(nqubits, i, H);
		}
	}

	void Shor::full_Shor(QuantumComputation& qc) {
		// TODO: Generate circuit

		QFT(qc, 0, N + 1);
		pADD(qc, 0, a);
		iQFT(qc, 0, N + 1);
	}

	/***
	 * Public Methods
	 ***/
	Shor::Shor(unsigned long n, unsigned long a) : N(n), a(a) {
		
		// Determine the bitsize of N
		while (N >> ++(size) > 0);

		// Set nr of Qubits/ClassicalBits
		addQubitRegister(2 * size + 3);
		addClassicalRegister(2 * size);

		// Circuit
		full_Shor(*this);
	}

	std::ostream& Shor::printStatistics(std::ostream& os) {
		// TODO: Adjust Statistics Output
		os << "Shor (" << nqubits << ") Statistics:\n";
		os << "\tn: " << nqubits + 1 << std::endl;
		os << "\tm: " << getNindividualOps() << std::endl;
		os << "--------------" << std::endl;
		return os;
	}
}