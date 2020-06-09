#include <cmath>
#include "algorithms/Shor.hpp"

// BernsteinVazirani without Entanglement
namespace qc {
	/***
	 * Private Methods
	 ***/

	// Set up all qubits for running the algorithm
	void Shor::setup(QuantumComputation& qc) {
		emplace_back<StandardOperation>(nqubits, 1, X);
	}

	void Shor::oracle(QuantumComputation& qc) {
		
		// Controlle Qubit for the C-Ua
		std::vector<Control> controls{};
		controls.emplace_back(0);

		// Vectors for measuring
		std::vector<unsigned short> qubit{};
		std::vector<unsigned short> singleMeasure{};
		std::vector<unsigned long> measure{};
		
		qubit.emplace_back(0);
		singleMeasure.emplace_back(0);

		int uaValue;
		for (int i = 0; i < 2 * size; i++)
		{
			// Create the value for this C-Ua
			uaValue = std::pow(a, std::pow(2, i));

			emplace_back<StandardOperation>(nqubits, 0, H);
			Ua(qc, controls, uaValue, 1);
			
			// Calculate the Shift
			long double lambda = 0;

			// Creating the QFT-Parts
			for (int power = i + 1, cbit = 0; power > 1; power--, cbit++)
			{
				long double powerOfTwo = std::pow(2.L, power);
				lambda = static_cast<fp>(PI / powerOfTwo);

				// TODO: Replace StandardOperation with ClassicControlledOperation
				emplace_back<StandardOperation>(nqubits, 0, RZ, lambda);
				//emplace_back<ClassicControlledOperation>(nqubits, measure[cbit], RZ, lambda);
			}
			emplace_back<StandardOperation>(nqubits, 0, H);
			
			// Measure
			// TODO: Correct implementation of the measure
			emplace_back<NonUnitaryOperation>(nqubits, qubit, singleMeasure);
			measure.emplace_back(singleMeasure[0]);

			// TODO: Replace StandardOperation with ClassicControlledOperation
			emplace_back<StandardOperation>(nqubits, 0, H);
			//emplace_back<ClassicControlledOperation>(nqubits, measure[i], H, lambda);
		}
	}

	void Shor::Ua(QuantumComputation& qc, std::vector<Control> controls, int value, int startQB) {
		CMULTmodN(qc, controls, value % N, startQB);

		// SWAP the x & b registers
		for (int i = startQB; i < startQB + size; ++i)
		{
			CSWAP(qc, controls, i, i + size);
		}
		QFT(qc, startQB + size, size + 1);
		ADDmodN(qc, {}, 0, startQB + size);
		iQFT(qc, startQB + size, size + 1);
		iCMULTmodN(qc, controls, iModN(value), startQB);
	}

	// Adds/Subtract(Inverse) a classical number to a QF-transformed number.
	// controls: The control bits for this adder
	// startQB: The first qubit in the Adder
	// value: The classical number
	// invert: true = subtract, false = add
	void Shor::ADD(QuantumComputation& qc, std::vector<Control> controls, int startQB, long value, bool invert)
	{
		// b + (-a) == b - (+a)
		if (value < 0)
		{
			invert = ~invert;
			value = -value;
		}

		std::bitset<64> aBitSet(value);
		int aSize = 0;

		for (unsigned int i = startQB; i <= startQB + size; ++i)
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
	void Shor::ADDmodN(QuantumComputation& qc, std::vector<Control> controlsOut, long value, int startQB)
	{
		// Define the controling Qubit inside the the ADDmodN-Block
		std::vector<Control> controlsIn{};
		controlsIn.emplace_back(startQB + size + 1);

		// Calculate (x + a) - N
		ADD(qc, controlsOut, startQB, value);
		ADD(qc, {}, startQB, N, true);
		iQFT(qc, startQB, size + 1);

		// Set Ancilla <=> (x + a) - N < 0
		emplace_back<StandardOperation>(nqubits, Control(startQB + size), startQB + size + 1, X);

		// (x + a) - N + N <=> (x + a) - N < 0
		QFT(qc, startQB, size + 1);
		ADD(qc, controlsIn, startQB, N);

		// Reset the Ancilla
		ADD(qc, controlsOut, startQB, value, true);
		iQFT(qc, startQB, size + 1);

		emplace_back<StandardOperation>(nqubits, startQB + size, X);
		emplace_back<StandardOperation>(nqubits, Control(startQB + size), startQB + size + 1, X);
		emplace_back<StandardOperation>(nqubits, startQB + size, X);

		QFT(qc, startQB, size + 1);
		ADD(qc, controlsOut, startQB, value);
	}

	void Shor::iADDmodN(QuantumComputation& qc, std::vector<Control> controlsOut, long value, int startQB)
	{
		// Define the controling Qubit inside the the ADDmodN-Block
		std::vector<Control> controlsIn{};
		controlsIn.emplace_back(startQB + size + 1);

		ADD(qc, controlsOut, startQB, value, true);
		iQFT(qc, startQB, size + 1);

		emplace_back<StandardOperation>(nqubits, startQB + size, X);
		emplace_back<StandardOperation>(nqubits, Control(startQB + size), startQB + size + 1, X);
		emplace_back<StandardOperation>(nqubits, startQB + size, X);

		QFT(qc, startQB, size + 1);
		ADD(qc, controlsOut, startQB, value);
		ADD(qc, controlsIn, startQB, N, true);
		iQFT(qc, startQB, size + 1);

		emplace_back<StandardOperation>(nqubits, Control(startQB + size), startQB + size + 1, X);
		QFT(qc, startQB, size + 1);
		ADD(qc, {}, startQB, N);
		ADD(qc, controlsOut, startQB, value, true);
	}


	// Calculates: (b + a*x) % N
	// startQB: The first qubit of the ADDmodN-Block without the contolling qubits
	void Shor::CMULTmodN(QuantumComputation& qc, std::vector<Control> controls, int value, int startQB)
	{
		QFT(qc, startQB + size, size + 1);
		for (int i = 0; i < size; ++i)
		{
			controls.emplace_back(startQB + i);
			ADDmodN(qc, controls, value % N, startQB + size);
			value *= 2;
			controls.pop_back();
		}
		iQFT(qc, startQB + size, size + 1);
	}

	void Shor::iCMULTmodN(QuantumComputation& qc, std::vector<Control> controls, int value, int startQB)
	{
		QFT(qc, startQB + size, size + 1);
		for (int i = 0; i < size; ++i)
		{
			controls.emplace_back(startQB + i);
			iADDmodN(qc, controls, value % N, startQB + size);
			value *= 2;
			controls.pop_back();
		}
		iQFT(qc, startQB + size, size + 1);
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

	void Shor::CSWAP(QuantumComputation& qc, std::vector<Control> controls, int qubitA, int qubitB)
	{
		emplace_back<StandardOperation>(nqubits, Control(qubitB), qubitA, X);

		controls.emplace_back(qubitA);
		emplace_back<StandardOperation>(nqubits, controls, qubitB, X);
		controls.pop_back();

		emplace_back<StandardOperation>(nqubits, Control(qubitB), qubitA, X);
	}

	void Shor::full_Shor(QuantumComputation& qc) 
	{
		setup(qc);
		oracle(qc);
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
		addQubitRegister(2 * size + 3);
		addClassicalRegister(2 * size + 3);

		// Circuit
		full_Shor(*this);
	}

	// Caluculating the modular inverse of a mod N
	// by an iterativ extended euclidean algorithm
	int Shor::iModN(int value)
	{
		int x = N, y = value;
		int ux = 1, vx = 0, uy = 0, vy = 1;
		int q, r;
		while (y != 1)
		{
			// Requirement not satisfied: gcd(N,a) == 1
			if (y == 0)
			{
				throw "gcd(N,a) > 1";
			}

			// Set quotient and remainder
			q = x / y;
			r = x % y;

			// Set new u & v values
			x = -uy * q + ux;
			ux = uy;
			uy = x;

			x = -vy * q + vx;
			vx = vy;
			vy = x;

			// Set new x & y
			x = y;
			y = r;
		}

		// Return the positiv modular inverse
		return vy < 0 ? vy + N : vy;
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