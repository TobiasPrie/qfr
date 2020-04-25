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
	void Shor::pADD(QuantumComputation& qc, int startQB, int nInputQB, unsigned long number, bool invert)
	{
		std::bitset<64> aBitSet(number);

		// Loop traversing from qubit x to x+n over n qubits, where n is the bitsize of N. 
		// The last additional qubit is to prevent overflow after the QFT. 
		for (unsigned int i = startQB; i < startQB + nInputQB; ++i) {

			// Precalculating the Phaseshift
			double phaseShift = 0;
			for (unsigned int j = i; j < startQB + nInputQB; ++j) {
				phaseShift += aBitSet[j - startQB] / std::pow(2, j - i);
			}
			phaseShift *= static_cast<fp>(PI);

			// Invert the Adder
			int m1 = 1;
			if (invert)
			{
				m1 = -1;
			}

			// Add the gate with the precalculated phaseShift = Ai
			emplace_back<StandardOperation>(nqubits, i, RZ, m1*phaseShift);
		}
	}

	void Shor::pADDmodN(QuantumComputation& qc) 
	{
		// TODO: Implement pADDmodN gate
	}

	// Quantum Fourier Transform block
	// startQB: The first qubit in the QFT
	// nInputQB: The number of qubits in the QFT
	void Shor::QFT(QuantumComputation& qc, int startQB, int nInputQB) {
		for (unsigned short i = startQB; i < startQB + nInputQB; ++i) {
			emplace_back<StandardOperation>(nqubits, i, H);
			for (unsigned short j = 1; j < nInputQB - (i - startQB); ++j) {
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
	// nInputQB: The number of qubits in the iQFT
	void Shor::iQFT(QuantumComputation& qc, int startQB, int nInputQB) {
		for (int i = startQB + nInputQB - 1; i >= startQB; --i) {
			for (unsigned short j = 1; j < nInputQB - (i - startQB); ++j) {
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

		QFT(qc, 2, 5);
		pADD(qc, 2, 5, a);
		iQFT(qc, 2, 5);
	}

	/***
	 * Public Methods
	 ***/
	Shor::Shor(unsigned long n, unsigned long a) : N(n), a(a) {
		
		// Determine the bitsize of N
		while (N >> ++(size) > 0);

		// Set nr of Qubits/ClassicalBits
		//addQubitRegister(2 * size + 3);
		//addClassicalRegister(2 * size);
		addQubitRegister(size+2);
		addClassicalRegister(size+2);

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