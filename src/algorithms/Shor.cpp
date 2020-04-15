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

	// The adder uses n+1 qubits starting from the starting qubit 
	// and add the number to the quantum register
	void Shor::pADD(QuantumComputation& qc, int startQB, unsigned long number) 
	{
		// Loop traversing from qubit x to x+n over n qubits, where n is the bitsize of N. 
		// The last additional qubit is to prevent overflow after the QFT. 
		for (unsigned short i = startQB; i < startQB + size; i++) {
			std::bitset<64> aBitSet(number);

			// Precalculating the Phaseshift
			double phaseShift = 0;
			for (unsigned short j = 0; j <= i - startQB; j--) {
				phaseShift += aBitSet[j] / std::pow(2, i - startQB - j);
			}
			phaseShift *= qc::PI;

			// Add the gate with the precalculated phaseShift = Ai
			emplace_back<StandardOperation>(nqubits, i, RZ, phaseShift);
		}
	}

	void Shor::pADDmodN(QuantumComputation& qc) 
	{
		// TODO: Implement pADDmodN gate
	}

	void Shor::iQFT(QuantumComputation& qc) {
		// TODO: Implement iQFT
	}

	void Shor::full_Shor(QuantumComputation& qc) {
		// TODO: Generate circuit
	}

	/***
	 * Public Methods
	 ***/
	Shor::Shor(unsigned long n, unsigned long a) : n(n) : a(a) {
		
		// Determine the bitsize of N
		while (hiddenInteger >> ++(size) > 0);

		// Set nr of Qubits/ClassicalBits
		addQubitRegister(2 * size + 3);
		addClassicalRegister(2 * size);

		// Circuit
		full_Shor(*this);
	}

	std::ostream& BernsteinVazirani::printStatistics(std::ostream& os) {
		// TODO: Adjust Statistics Output
		os << "Shor (" << nqubits << ") Statistics:\n";
		os << "\tn: " << nqubits + 1 << std::endl;
		os << "\tm: " << getNindividualOps() << std::endl;
		os << "--------------" << std::endl;
		return os;
	}
}