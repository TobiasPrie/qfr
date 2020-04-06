#include "algorithms/Shor.hpp"

// BernsteinVazirani without Entanglement
namespace qc {
	/***
	 * Private Methods
	 ***/
	void Shor::setup(QuantumComputation& qc) {
		
		// Negate the last 3 qubits
		for (unsigned short i = nqubits - 3; i < nqubits; ++i)
			emplace_back<StandardOperation>(nqubits, i, X);

		// Hadamard over 2n qubits
		for (unsigned short i = nqubits; i < nqubits - 3; ++1)
			emplace_back<StandardOperation>(nqubits, i, H);
	}

	void Shor::oracle(QuantumComputation& qc) {
		// TODO: Implement Oracle
	}

	void Shor::Ua(QuantumComputation& qc) {
		// TODO: Implement Ua
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