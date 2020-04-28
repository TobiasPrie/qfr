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

	// Adds/Subtract(Inverse) a classical number to a QF-transformed number.
	// controls: The control bits for this adder
	// startQB: The first qubit in the Adder
	// value: The classical number
	// invert: true = subtract, false = add
	void Shor::pADD(QuantumComputation& qc, std::vector<Control> controls, int startQB, unsigned long value, bool invert)
	{
		std::bitset<64> aBitSet(value);

		// Loop traversing from qubit x to x+n over n qubits, where n is the bitsize of N. 
		// The last additional qubit is to prevent overflow after the QFT. 
		for (unsigned int i = startQB; i < startQB + size + 1; ++i)
		{
			double phaseShift = 0;
			for (unsigned int j = startQB; j < startQB + size; ++j) {
				phaseShift += aBitSet[j - startQB] / std::pow(2, i - j);
			}
			phaseShift *= static_cast<fp>(PI);

			// Invert the Adder
			int m1 = 1;
			if (invert)
			{
				m1 = -1;
			}

			// Add the gate with the precalculated phaseShift = Ai
			emplace_back<StandardOperation>(nqubits, controls, i, RZ, m1 * phaseShift);
		}
	}

	// Calculates: (Input + a) % N with controlling qubits
	// controlsOut: The control bits for this ADDmodN-Block
	// value: (Input + value) % N
	// startQB: The first qubit of the ADDmodN-Block without the contolling qubits
	void Shor::pADDmodN(QuantumComputation& qc, std::vector<Control> controlsOut, unsigned long value, int startQB)
	{
		// Define the controling Qubit inside the the ADDmodN-Block
		std::vector<Control> controlsIn{};
		controlsIn.emplace_back(startQB + size + 1);
		
		// Calculate (x + a) - N
		pADD(qc, controlsOut, startQB, value);
		pADD(qc, {}, startQB, N, true);
		iQFT(qc, 2, size + 1);

		// Set Ancilla <=> (x + a) - N < 0
		emplace_back<StandardOperation>(nqubits, Control(startQB + size), startQB + size + 1, X);

		// (x + a) - N + N <=> (x + a) - N < 0
		QFT(qc, startQB, size + 1);
		pADD(qc, controlsIn, startQB, N);

		// Reset the Ancilla
		pADD(qc, controlsOut, startQB, value, true);
		iQFT(qc, startQB, size + 1);

		emplace_back<StandardOperation>(nqubits, startQB + size, X);
		emplace_back<StandardOperation>(nqubits, Control(startQB + size), startQB + size + 1, X);
		emplace_back<StandardOperation>(nqubits, startQB + size, X);

		QFT(qc, startQB, size + 1);
		pADD(qc, controlsOut, startQB, value);
	}

	// Quantum Fourier Transform block
	// startQB: The first qubit in the QFT
	// nInputQB: The number of qubits in the QFT
	void Shor::iQFT(QuantumComputation& qc, int startQB, int nInputQB) {

		for (unsigned short i = startQB; i < startQB + nInputQB; ++i) {
			emplace_back<StandardOperation>(nqubits, i, H);
			for (unsigned short j = 1; j < nInputQB - (i - startQB); ++j) {
				long double powerOfTwo = std::pow(2.L, j);
				auto lambda = static_cast<fp>(PI / powerOfTwo);
				emplace_back<StandardOperation>(nqubits, Control(i + j), i, RZ, -lambda);
			}
		}
	}

	// Inverse Quantum Fourier Transform block
	// startQB: The first qubit in the iQFT
	// nInputQB: The number of qubits in the iQFT
	void Shor::QFT(QuantumComputation& qc, int startQB, int nInputQB) {

		for (int i = startQB + nInputQB - 1; i >= startQB; --i) {
			for (unsigned short j = 1; j < nInputQB - (i - startQB); ++j) {
				long double powerOfTwo = std::pow(2.L, j);
				auto lambda = static_cast<fp>(PI / powerOfTwo);
				emplace_back<StandardOperation>(nqubits, Control(i + j), i, RZ, lambda);
			}
			emplace_back<StandardOperation>(nqubits, i, H);
		}
	}

	void Shor::full_Shor(QuantumComputation& qc) {
		// TODO: Generate circuit
		std::vector<Control> controls{};
		controls.emplace_back(0);
		controls.emplace_back(1);
		emplace_back<StandardOperation>(nqubits, 0, X);
		emplace_back<StandardOperation>(nqubits, 1, X);
		QFT(qc, 2, size + 1);
		pADDmodN(qc, controls, a, 2);
		iQFT(qc, 2, size + 1);
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
		addQubitRegister(size+4);
		addClassicalRegister(size+4);

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