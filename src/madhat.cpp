////////////////////////////////////////////////////////////
// MADHAT: Model-Agnostic Dark Halo Analysis Tool
//
// Date Last Modified: 10/01/2019
// Version Number: 1.0
////////////////////////////////////////////////////////////

#include <iostream>
#include <fstream>
#include <sstream>
#include <time.h>
#include <string>
#include <iomanip>
#include <sys/stat.h>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>


using boost::multiprecision::cpp_dec_float;
typedef boost::multiprecision::number < cpp_dec_float < 50 >> madfloat;

using namespace std;
using namespace boost;

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif
/////////////////////////////////////////////////////////////////////////////////////////////////
bool fileout;
bool printbeta;
int gal = 58; //The number of dwarf galaxies in the data files    
/////////////////////////////////////////////////////////////////////////////////////////////////
//dist() Finds the Probability of TOT DM
float dist(int Ndm, float Nbound) {
    float P2; //The float that will end out being the probability
    P2 = exp(Ndm * log(Nbound) - Nbound - lgamma(Ndm + 1));
    return P2; //Return the value
}
/////////////////////////////////////////////////////////////////////////////////////////////////
//READING DATA FROM DAT FILES
// Read Set
float process(float Beta, std::ifstream & INPUT, float mass, float energy, int Juse = 1, int Jerror = 1, int intype = 1) {
    float crosscons = 8 * M_PI * mass * mass / energy; //Creates a constant that is used to calculate the cross section

    int dwarf_count = 0;
    int dcol = 0;
    string line;
    string item;
    int header = 0;
    string skip("#");
    INPUT.seekg(0, ios::beg);
    while (getline(INPUT, line)) {
        if (contains(line, skip)) {
            header++;
        } else {
            break;
        }
    }

    INPUT.seekg(0, ios::beg);
    for (int i = 0; i < header; ++i) {
        getline(INPUT, line);

    }
    while (getline(INPUT, line)) {
        ++dwarf_count;
        if (dwarf_count == 1) {
            stringstream ss(line);
            while (ss >> item)
                dcol++;
        }
    }
    INPUT.clear();
    INPUT.seekg(0, ios::beg);
    for (int i = 0; i < header; ++i) {
        getline(INPUT, line);
    }
    float dwarf_data[dwarf_count][4];

    float check;
    for (int i = 0; i != dwarf_count; ++i) {
        dwarf_data[i][0] = 0;
        dwarf_data[i][1] = 0;
        dwarf_data[i][2] = 0;
        dwarf_data[i][3] = 0;
        getline(INPUT, line);
        istringstream read(line);
        while (read >> check) {
            read.seekg(0, ios::beg);
            read >> dwarf_data[i][0];
            read >> dwarf_data[i][1];



            read >> dwarf_data[i][2];
           


  if (dwarf_data[i][2] < 0){
freopen ("/dev/tty", "a", stdout);
        printf("Error: Invalid +dJ for dwarfs in .dat file.\n");
        exit(1);
       }

            read >> dwarf_data[i][3];



 if (dwarf_data[i][3] < 0){
freopen ("/dev/tty", "a", stdout);
        printf("Error: Invalid -dJ for dwarfs in .dat file.\n");
        exit(1);
       }


            break;
        }
    }
    INPUT.close();
    // Got SetTemplate
    // Get Obseravation data
    ifstream NOBS;
    NOBS.open("PMFdata/Fermi_Pass8R3_239557417_585481831/NOBS.dat");
    if (!NOBS) {
freopen ("/dev/tty", "a", stdout);
        printf("Error: Failed to load dwarf data file.\n");
        exit(1);
    }
    header = 0;
    NOBS.seekg(0, ios::beg);
    while (getline(NOBS, line)) {
        if (contains(line, skip)) {
            header++;
        } else {
            break;
        }
    }
    NOBS.seekg(0, ios::beg);
    for (int i = 0; i < header; ++i) {
        getline(NOBS, line);
    }

    float obs_data[gal][3];
    for (int i = 0; i != gal; ++i) {
        for (int j = 0; j != 3; ++j) {
            NOBS >> obs_data[i][j];
        }
    }
    NOBS.close();
    // Got Observation Data
    // Get pmf
    ifstream PMF;
    PMF.open("PMFdata/Fermi_Pass8R3_239557417_585481831/pmf.dat");
    if (!PMF) {
freopen ("/dev/tty", "a", stdout);
        printf("Error: Failed to load Fermi data.\n");
        exit(1);
    }
    header = 0;
    PMF.seekg(0, ios::beg);
    while (getline(PMF, line)) {
        if (contains(line, skip)) {
            header++;
        } else {
            break;
        }
    }
    PMF.seekg(0, ios::beg);
    for (int i = 0; i < header; ++i) {
        getline(PMF, line);
    }
    vector < vector < float >> pmf(20000, vector < float > (gal + 1));
    for (auto & i: pmf)
        std::fill(i.begin(), i.end(), 0);
    for (int i = 0; i != 1200; ++i) {
        for (int j = 0; j != gal + 1; ++j) {
            PMF >> pmf[i][j];
        }
    }
    PMF.close();

    // Got pmf
    /////////////////////////////////////////////////////////////////////////////////////////////////
    //Classifying Dwarf Information
    int dwarf_list[dwarf_count]; //A list of dwarfs used by number
    for (int i = 0; i < dwarf_count; i++) {
        if (floorf(dwarf_data[i][0]) == dwarf_data[i][0] && dwarf_data[i][0] != 0 && dwarf_data[i][0] <= gal) //Checking if all dwarfs listed are grabable 
        {
            dwarf_list[i] = dwarf_data[i][0]; //importing the dwarf list for lookup
        } else {
freopen ("/dev/tty", "a", stdout);
            printf("Error: Not all dwarfs listed in the set were found. Please check dwarf ID numbers in the README.\n");
            exit(1);
        }
    }
    int Nobs = 0; // total number of events observed
    for (int i = 0; i < dwarf_count; i++) {
        Nobs = Nobs + obs_data[dwarf_list[i] - 1][1]; //Summing over all observed events
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////
    //Calculating P1
    int Nbgd = 0;
    float P1[Nobs + 1];
    memset(P1, 0, sizeof(P1));
    if (dwarf_count == 1) { //For Number of dwarfs=1 simply gathering them from the file
        for (int i = 0; i < (Nobs + 1); i++) {
            P1[i] = pmf[i][dwarf_list[0]];
        }
    }
    if (dwarf_count != 1) { //Incrementing up Ntot Bgd while summing over partitions
        float tempp = 0;
        float X[Nobs + 2];
        float I[Nobs + 1];

        for (int i = 0; i < Nobs + 1; i++) {
            I[i] = pmf[i][dwarf_list[0]];
        }
        for (int i = 0; i < Nobs + 2; i++) {
            X[i] = 0;
        }
        for (int k = 1; k < (dwarf_count - 1); k++) {
            for (int j = 0; j < Nobs + 2; j++) {
                for (int i = 0; i < j + 1; i++) {
                    tempp = pmf[i][dwarf_list[k]] * I[j - i];
                    X[j] = X[j] + tempp;
                }
            }
            for (int j = 0; j < Nobs + 1; j++) {
                I[j] = X[j];
            }
            for (int j = 0; j < Nobs + 1; j++) {
                X[j] = 0;
            }
        }
        for (int n = 0; n < (Nobs + 1); n++) {
            for (int k = 0; k < n + 1; k++) {
                X[k] = pmf[n - k][dwarf_list[dwarf_count - 1]] * I[k];
            }
            for (int k = 0; k < n + 1; k++) {
                P1[n] = P1[n] + X[k];
            }
            Nbgd++;
        }
    }
    Nbgd = 0;
    /////////////////////////////////////////////////////////////////////////////////////////////////
    //Prepping J factors for calculation
    madfloat JAT = 0; //Variable for J factor times Aeff*Tobs
    madfloat JP = 0; //Variable for J factor times Aeff*Tobs + error
    madfloat JM = 0; //Variable for J factor times Aeff*Tobs -error
    madfloat SJAT = 0; //sum of J factors times Aeff*Tobs
    madfloat SJATP = 0; //+error sum of J factors times Aeff*Tobs
    madfloat SJATM = 0; //-error sum of J factors times Aeff*Tobs
    madfloat J = 0; //Jfactor
    if (Juse == 1) {
        for (int i = 0; i < dwarf_count; ++i) {
            J = pow(10, dwarf_data[i][1]); //Undoing the log base 10
            JAT = J * obs_data[dwarf_list[i] - 1][2]; //Multiplying by Aeff*Tobs
            SJAT = SJAT + JAT; //summing them up in the loop
            if (Jerror == 1) {
                JP = pow(10, dwarf_data[i][1] + dwarf_data[i][2]); //Taking the +error in log10(J) and undoing the log base 10
                JP = JP * obs_data[dwarf_list[i] - 1][2]; //Then multiplying by Aeff*Tobs
                SJATP = SJATP + JP; //summing those errors up
                // Then repeating the same process with M for minus
                JM = pow(10, dwarf_data[i][1] - dwarf_data[i][3]); //Taking the +error in log10(J) and undoing the log base 10
                JM = JM * obs_data[dwarf_list[i] - 1][2]; //Then multiplying by Aeff*Tobs
                SJATM = SJATM + JM; //summing those errors up
            }
        }
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////
    //Setting the headers for the output files
    if (Juse == 1 && Jerror == 1 && crosscons != 0 && mass == 0) //Has J factors Has J factor errors Has DM mass
    {
        printf("#Beta      Nbound        PhiPP(cm^3 s^-1 GeV^-2)   +dPhiPP             -dPhiPP            sigv(cm^3 s^-1)        +dsigv             -dsigv\n");
    }
    if (Juse == 1 && Jerror == 0 && crosscons != 0 && mass == 0) //Has J factors Has J factor errors Has DM mass
    {
        printf("#Beta      Nbound        PhiPP(cm^3 s^-1 GeV^-2)   sigv(cm^3 s^-1)\n");
    }
    if (Juse == 1 && Jerror == 1 && crosscons == 0 && mass == 0) //Has J factors Has J factor errors No DM mass
    {
        printf("#Beta      Nbound        PhiPP(cm^3 s^-1 GeV^-2)   +dPhiPP             -dPhiPP\n");
    }
    if (Juse == 1 && Jerror == 0 && crosscons == 0 && mass == 0) //Has J factors No J factor errors No DM mass
    {
        printf("#Beta      Nbound        PhiPP(cm^3 s^-1 GeV^-2)\n");
    }
    if (Juse == 0 && mass == 0) //No J factors No J factor errors No DM mass
    {
        printf("#Beta      Nbound\n");
    }
    if (Juse == 1 && Jerror == 1 && crosscons != 0 && mass != 0) //Has J factors Has J factor errors Has DM mass
    {
        printf("#Mass(GeV)   Spectrum       Beta      Nbound        PhiPP(cm^3 s^-1 GeV^-2)   +dPhiPP             -dPhiPP            sigv(cm^3 s^-1)        +dsigv             -dsigv\n");
    }
    if (Juse == 1 && Jerror == 0 && crosscons != 0 && mass != 0) //Has J factors Has J factor errors Has DM mass
    {
        printf("#Mass(GeV)   Spectrum       Beta      Nbound        PhiPP(cm^3 s^-1 GeV^-2)   sigv(cm^3 s^-1)\n");
    }
    if (Juse == 1 && Jerror == 1 && crosscons == 0 && mass != 0) //Has J factors Has J factor errors No DM mass
    {
        printf("#Mass(GeV)   Spectrum       Beta      Nbound        PhiPP(cm^3 s^-1 GeV^-2)   +dPhiPP             -dPhiPP\n");
    }
    if (Juse == 1 && Jerror == 0 && crosscons == 0 && mass != 0) //Has J factors No J factor errors No DM mass
    {
        printf("#Mass(GeV)   Spectrum       Beta      Nbound        PhiPP(cm^3 s^-1 GeV^-2)\n");
    }
    if (Juse == 0 && mass != 0) //No J factors No J factor errors No DM mass
    {
        printf("#Mass(GeV)      Spectrum       Beta      Nbound       \n");
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////
    //final summation loop

    float Pf = 0; //The complement of Beta
    float B = 0; //Beta  
    float P3;
    float PP;
    float Nbound = 1;
    for (Nbound = 1; Nbound < (Nobs + 1) && B < Beta; Nbound++) {

        int Ndm = 0;
        int Nbgd = Nobs; //Setting Ndm=Nobs
        Pf = 0; //Zeroing the placeholder for the sum
        float exp; //A variable to store the exponential
        for (int i = 0; i < (Nobs + 1); i++) //Summing over Nbgd+Ndm<NObs
        {
            P3 = 1;
            PP = 0;
            if (P1[Nbgd] > 0) {
                for (Ndm = 0;
                    (Ndm < (i + 1) && P3 > (0)) || PP == 0; Ndm++) {
                    P3 = dist(Ndm, Nbound);
                    Pf = Pf + (P1[Nbgd] * P3); //Summing over the probability
                    PP = PP + P3;
                }

            }

            Nbgd--;
        }

        B = 1 - Pf; //Taking the complement of the sum

        // compare B, beta variable which is increasing toward "Beta", entered by user, where Nbound(beta)=1 with "Beta" and if it is larger, says beta is smaller than the minimum
        if (Nbound == 1 && B - Beta > 0) {
freopen ("/dev/tty", "a", stdout);
            printf("Error: The value of beta you have entered is too small. Please choose a larger value of beta.\n");
            exit(1);
        }

        if (printbeta == true && intype != 3) {
            madfloat PHI = Nbound; //A variable for PHI
            madfloat PHIP = Nbound; //A variable for PHI + error 
            madfloat PHIM = Nbound; //A variable for PHI - error
            madfloat CS; //A variable for CS
            madfloat CSP; //A variable for CS + error 
            madfloat CSM; //A variable for CS - error
            if (Juse == 1) {
                PHI = PHI / SJAT; //Phi=Nbound/JAT calculated earlier
            }

            if (Jerror == 1) {
                PHIP = PHIP / SJATP; //Phi=Nbound/JAT calculated earlier
                PHIM = PHIM / SJATM; //Phi=Nbound/JAT calculated earlier
                CS = PHI * crosscons;
                CSP = PHIP * crosscons;
                CSM = PHIM * crosscons;
            }

            if (mass != 0) {
                printf("%.4f     %.4f      ", mass, energy); //Print Nbound and Beta
            }
            printf("%.4f     %.4f      ", B, Nbound); //Print Nbound and Beta
            if (Juse == 1) {
                cout << PHI; //Print Phi if J factors
            }
            if (Jerror == 1) {
                printf("            ");
                cout << PHIM - PHI; //Print Phi error if J factors have error
                printf("        ");
                cout << -(PHIP - PHI);
            }
            if (crosscons != 0) {
                if (Juse == 1) {
                    printf("            ");
                    cout << CS; //Print Phi if J factors
                }
                if (Jerror == 1) {
                    printf("        ");
                    cout << CSM - CS; //Print Phi error if J factors have error
                    printf("        ");
                    cout << -(CSP - CS);
                }
            }
            printf("\n");
        }

    }

    //Final Calculation
    Nbound = Nbound - 2; //Decrementing back one to find the bound
    B = 0;
    //Loop counter that does the same as final summation but goes down decimals in precision
    for (int cut = 1; cut < 10; cut++) //cut is the decimal point
    {
        if (cut > 1) {
            Nbound = Nbound + pow(10, -cut - 1);
        }
        for (int acc = 0; acc < 9 && B < Beta; acc++) { //loops over .1,.2,.3...to .9
            int Ndm = 0;
            int Nbgd = Nobs; //Setting Ndm=Nobs
            Pf = 0; //Zeroing the placeholder for the sum
            float exp; //A variable to store the exponential
            for (int i = 0; i < (Nobs + 1); i++) //Summing over Nbgd+Ndm<NObs
            {
                P3 = 1;
                PP = 0;
                if (P1[Nbgd] > 0) {
                    for (Ndm = 0;
                        (Ndm < (i + 1) && P3 > (0)) || PP == 0; Ndm++) {
                        P3 = dist(Ndm, Nbound);
                        Pf = Pf + (P1[Nbgd] * P3); //Summing over the probability
                        PP = PP + P3;
                    }
                }
                Nbgd--;
            }
            B = 1 - Pf; //Taking the complement of the sum
            Nbound = Nbound + pow(10, -cut);

        }
    }
    int Ndm = 0;
    Nbgd = Nobs; //Setting Ndm=Nobs
    Pf = 0; //Zeriong the placeholder for the sum
    float exp; //A variable to store the exponential
    for (int i = 0; i < (Nobs + 1); i++) //Summing over Nbgd+Ndm<NObs
    {
        P3 = 1;
        PP = 0;
        if (P1[Nbgd] > 0) {
            for (Ndm = 0;
                (Ndm < (i + 1) && P3 > (0)) || PP == 0; Ndm++) {
                P3 = dist(Ndm, Nbound);
                Pf = Pf + (P1[Nbgd] * P3); //Summing over the probability
                PP = PP + P3;
            }
        }
        Nbgd--;
    }
    B = 1 - Pf; //Taking the complement of the sum

    if (printbeta == false || intype == 3) {
        madfloat PHI = Nbound; //A variable for PHI
        madfloat PHIP = Nbound; //A variable for PHI + error 
        madfloat PHIM = Nbound; //A variable for PHI - error
        madfloat CS; //A variable for CS
        madfloat CSP; //A variable for CS + error 
        madfloat CSM; //A variable for CS - error
        if (Juse == 1) {
            PHI = PHI / SJAT; //Phi=Nbound/JAT calculated earlier
        }
        if (Jerror == 1) {
            PHIP = PHIP / SJATM; //Phi=Nbound/JAT calculated earlier
            PHIM = PHIM / SJATP; //Phi=Nbound/JAT calculated earlier
            CS = PHI * crosscons;
            CSP = PHIP * crosscons;
            CSM = PHIM * crosscons;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////
        //Final Output
        if (mass != 0) {
            printf("%.4f     %.4f      ", mass, energy); //Print Nbound and Beta
        }
        printf("%.4f     %.4f      ", B, Nbound); //Print Nbound and Beta
        if (Juse == 1) {
            cout << PHI; //Print Phi if J factors
        }
        if (Jerror == 1) {
            printf("            ");
            cout << PHIP - PHI; //Print Phi error if J factors have error
            printf("        ");
            cout << -(PHIM - PHI);
        }
        if (crosscons != 0) {
            if (Juse == 1) {
                printf("        ");
                cout << CS; //Print Phi if J factors
            }
            if (Jerror == 1) {
                printf("        ");
                cout << CSP - CS; //Print Phi error if J factors have error
                printf("        ");
                cout << -(CSM - CS);
            }
        }
    }
    printf("\n");
    Beta = B;
    return Nbound;
}
/////////////////////////////////////////////////////////////////////////////////////////////////
// This is a duplicate of the main process if Nbound is already found, which is the bulk of the calculation. It exist for looping outputs
int output(float Nbound, float B, std::ifstream & INPUT, float mass, float energy, int Juse = 1, int Jerror = 1, int intype = 1) {
    float crosscons = 8 * M_PI * mass * mass / energy; //Creates a constant that is used to calculate the cross section   
    int dwarf_count = 0;
    int dcol = 0;
    string line;
    string item;
    int header = 0;
    string skip("#");
    INPUT.seekg(0, ios::beg);
    while (getline(INPUT, line)) {
        if (contains(line, skip)) {
            header++;
        } else {
            break;
        }
    }
    INPUT.seekg(0, ios::beg);
    for (int i = 0; i < header; ++i) {
        getline(INPUT, line);
    }
    while (getline(INPUT, line)) {
        ++dwarf_count;
        if (dwarf_count == 1) {
            stringstream ss(line);
            while (ss >> item)
                dcol++;
        }
    }
    INPUT.clear();
    INPUT.seekg(0, ios::beg);
    for (int i = 0; i < header; ++i) {
        getline(INPUT, line);
    }
    float dwarf_data[dwarf_count][4];

    float check;
    for (int i = 0; i != dwarf_count; ++i) {
        dwarf_data[i][0] = 0;
        dwarf_data[i][1] = 0;
        dwarf_data[i][2] = 0;
        dwarf_data[i][3] = 0;
        getline(INPUT, line);
        istringstream read(line);
        while (read >> check) {
            read.seekg(0, ios::beg);
            read >> dwarf_data[i][0];
            read >> dwarf_data[i][1];
            read >> dwarf_data[i][2];
            read >> dwarf_data[i][3];
            break;
        }
    }
    INPUT.close();
    // Got SetTemplate
    // Get Obseravation data
    ifstream NOBS;
    NOBS.open("PMFdata/Fermi_Pass8R3_239557417_585481831/NOBS.dat");
    if (!NOBS) {
freopen ("/dev/tty", "a", stdout);
        printf("Error: Failed to load dwarf data file.\n");
        exit(1);
    }
    header = 0;
    NOBS.seekg(0, ios::beg);
    while (getline(NOBS, line)) {
        if (contains(line, skip)) {
            header++;
        } else {
            break;
        }
    }
    NOBS.seekg(0, ios::beg);
    for (int i = 0; i < header; ++i) {
        getline(NOBS, line);
    }
    float obs_data[gal][3];
    memset(obs_data, 0, sizeof(obs_data));
    for (int i = 0; i != gal; ++i) {
        for (int j = 0; j != 3; ++j) {
            NOBS >> obs_data[i][j];
        }
    }
    NOBS.close();
    // Got Observation Data
    // Get pmf
    ifstream PMF;
    PMF.open("PMFdata/Fermi_Pass8R3_239557417_585481831/pmf.dat");
    if (!PMF) {
freopen ("/dev/tty", "a", stdout);
        printf("Error: Failed to load dwarf data file.\n");
        exit(1);
    }
    header = 0;
    PMF.seekg(0, ios::beg);
    while (getline(PMF, line)) {
        if (contains(line, skip)) {
            header++;
        } else {
            break;
        }
    }
    PMF.seekg(0, ios::beg);
    for (int i = 0; i < header; ++i) {
        getline(PMF, line);
    }
    vector < vector < float >> pmf(20000, vector < float > (gal + 1));
    for (auto & i: pmf)
        std::fill(i.begin(), i.end(), 0);
    for (int i = 0; i != 1200; ++i) {
        for (int j = 0; j != gal + 1; ++j) {
            PMF >> pmf[i][j];
        }
    }
    PMF.close();
    // Got pmf
    /////////////////////////////////////////////////////////////////////////////////////////////////
    //Classifiying Dwarf Information
    int dwarf_list[dwarf_count]; //A list of dwarfs used by ID number
    for (int i = 0; i < dwarf_count; i++) {
        dwarf_list[i] = dwarf_data[i][0];
    }
    int Nobs = 0; //Number of observered over the set of dwarves
    for (int i = 0; i < dwarf_count; i++) {
        Nobs = Nobs + obs_data[dwarf_list[i] - 1][1]; //Summing over all observed events
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////
    //Prepping J factors for calculation
    madfloat JAT = 0; //Variable for J factor times Aeff*Tobs
    madfloat JP = 0; //Variable for J factor times Aeff*Tobs + error
    madfloat JM = 0; //Variable for J factor times Aeff*Tobs -error
    madfloat SJAT = 0; //sum of J factors times Aeff*Tobs
    madfloat SJATP = 0; //+error sum of J factors times Aeff*Tobs
    madfloat SJATM = 0; //-error sum of J factors times Aeff*Tobs
    madfloat J = 0; //Jfactor
    if (Juse == 1) {
        for (int i = 0; i < dwarf_count; ++i) {
            J = pow(10, dwarf_data[i][1]); //Undoing the log base 10
            JAT = J * obs_data[dwarf_list[i] - 1][2]; //Multiplying by Aeff*Tobs
            SJAT = SJAT + JAT; //summing them up in the loop
            if (Jerror == 1) {
                JP = pow(10, dwarf_data[i][1] + dwarf_data[i][2]); //Undoing the log base 10
                JP = JP * obs_data[dwarf_list[i] - 1][2]; //Then multiplying by Aeff*Tobs
                SJATP = SJATP + JP; //summing those errors up
                // Then repeating the same process with M for minus
                JM = pow(10, dwarf_data[i][1] - dwarf_data[i][3]); //Taking the -error in log10(J) and undoing the log base 10
                JM = JM * obs_data[dwarf_list[i] - 1][2]; //Then multiplying by Aeff*Tobs
                SJATM = SJATM + JM; //summing those errors up
            }
        }
    }
    if (printbeta == false || intype == 3) {
        madfloat PHI = Nbound; //A variable for PHI
        madfloat PHIP = Nbound; //A variable for PHI + error 
        madfloat PHIM = Nbound; //A variable for PHI - error
        madfloat CS; //A variable for CS
        madfloat CSP; //A variable for CS + error 
        madfloat CSM; //A variable for CS - error
        if (Juse == 1) {
            PHI = PHI / SJAT; //Phi=Nbound/JAT calculated earlier
        }
        if (Jerror == 1) {
            PHIP = PHIP / SJATM; //Phi=Nbound/JAT calculated earlier
            PHIM = PHIM / SJATP; //Phi=Nbound/JAT calculated earlier
            CS = PHI * crosscons;
            CSP = PHIP * crosscons;
            CSM = PHIM * crosscons;
        }
        /////////////////////////////////////////////////////////////////////////////////////////////////
        //Final Output
        if (mass != 0) {
            printf("%.4f     %.4f      ", mass, energy); //Print Nbound and Beta
        }
        printf("%.4f     %.4f      ", B, Nbound); //Print Nbound and Beta
        if (Juse == 1) {
            cout << PHI; //Print Phi if J factors
        }
        if (Jerror == 1) {
            printf("            ");
            cout << PHIP - PHI; //Print Phi error if J factors have error
            printf("        ");
            cout << -(PHIM - PHI);
        }
        if (crosscons != 0) {
            if (Juse == 1) {
                printf("        ");
                cout << CS; //Print Phi if J factors
            }
            if (Jerror == 1) {
                printf("        ");
                cout << CSP - CS; //Print Phi error if J factors have error
                printf("        ");
                cout << -(CSM - CS);
            }
        }
    }
    printf("\n");
    return 1;
}
/////////////////////////////////////////////////////////////////////////////////////////////////
int input(char * dat, float Beta) {
    //Telling it about your input file
    int Juse = 1; //Variable to know if there are J factors
    int Jerror = 1; //Variable to know if there are J factors have error
    char set[10]; //Input for which set to use
    char setJ[10]; //Input to know if there are J factors
    char setJE[10]; //Input to know if J factors have error

    string name;
    name = dat;
    string dwset = "Input/";
    dwset.append(name);

    ifstream INPUT;

    INPUT.open(dwset);
    if (!INPUT) {
freopen ("/dev/tty", "a", stdout);
        printf("Error: Failed to load dwarf data file.\n");
        exit(1);
    }
    float mass = 0;
    float energy = 1;
    float crosscons = 8 * M_PI * mass * mass / energy; //Creates a constant that is used to calculate the cross section
    if (fileout == true) {

        //check if "Output" directory exists, if does not, create "Output" directory
        struct stat st = {
            0
        };
        if (stat("Output", & st) == -1) {
            mkdir("Output", 0700);
        }

        string outst = "Output/"; //create output string
        string name;
        name = dat;
        boost::erase_all(name, "/");
        outst += name; //Add set name
        outst = outst.substr(0, outst.size() - 4); //remove set file extension
        outst += "_"; //Add underscore
        outst = outst + boost::lexical_cast < std::string > (boost::format("%.4f") % Beta); //Add confidence
        // time_t timer;
        // outst = outst + boost::lexical_cast < std::string > (boost::format("%.4f") % time( & timer)); //Add confidence
        outst += ".out"; //Add .out
        cout << "Working … ";
        cout << "File ";
        cout << outst;
        cout << " will be created shortly." << endl;
        const char * out = outst.c_str(); //setting the output string to the file name
        stdout = freopen(out, "w", stdout); // Setting the the print to the output file

        string address = "Input"; //copies the dwarfset data into output
        address += "/";
        address += name;
        ifstream useddwarfset(address); //address is the location of dwarfset.dat file

        int counter = 0; //counter counts the number of the lines in the header of the dwarfset.dat file
        string skip("#");
        string linef;
        useddwarfset.seekg(0, ios::beg);
        while (getline(useddwarfset, linef)) {
            if (contains(linef, skip)) {
                counter++;
                cout << linef << endl;

            } else {
                break;
            }
        }

        int numlines = 0; //numline counts the number of the total lines of the dwarfset.dat file
        useddwarfset.seekg(0, ios::beg);
        while (getline(useddwarfset, linef))
            numlines++;
        useddwarfset.clear();

        //we add "#" at the beginning of all the lines of dwarfset.dat file after the header, from line number (counter+1) until line (numline)
        for (int j = 1; j < numlines - counter + 1; ++j) {
            useddwarfset.seekg(0, ios::beg);
            for (int i = 0; i < counter + j; ++i) {
                getline(useddwarfset, linef);

            }
            string linef2("#");
            linef2 += linef;
            cout << linef2 << endl;
        }
        cout << "#####################################################################################################################################################################" << endl;
        cout << "# Fermi_Pass8R3_239557417_585481831" << endl;
        cout << "###################################################################################OUTPUT############################################################################" << endl;

    }

    process(Beta, INPUT, mass, energy, Juse, Jerror, 2);
    return 0;
}
/////////////////////////////////////////////////////////////////////////////////////////////////
int input(char * dat, float Beta, char * in ) {
    //Telling it about your input file
    //Reading the in file
    int line_count = 0;
    int dcol = 0;
    string line;
    string item;
    ifstream INPUT;
    ifstream MASS;
    string inputname;
    inputname = in ; // picks up the location of the file with "in" extension, e.g. Input/filename.in

    if (printbeta == true) {
        cout << "Warning: The Output Will Not be a table of Nbound(beta)" << endl;
    }

    string dmmass = "Input/";
    dmmass.append(inputname);

    MASS.open(dmmass);

    if (!MASS) {
freopen ("/dev/tty", "a", stdout);
        printf("Error: Failed to load dark matter parameters.\n");
        exit(1);
    }
    int header = 0;
    string skip("#");
    MASS.seekg(0, ios::beg);
    while (getline(MASS, line)) {
        if (contains(line, skip)) {
            header++;
        } else {
            break;
        }
    }

    MASS.seekg(0, ios::beg);
    for (int i = 0; i < header; ++i) {
        getline(MASS, line);
    }
    while (getline(MASS, line)) {
        ++line_count;
        if (line_count == 1) {
            stringstream ss(line);
        }
    }
    MASS.clear();
    MASS.seekg(0, ios::beg);
    for (int i = 0; i < header; ++i) {
        getline(MASS, line);

    }
    float mass_data[line_count][2];
    memset(mass_data, 0, sizeof(mass_data));
    float check;
    for (int i = 0; i != line_count; ++i) {
        mass_data[i][0] = 0;
        mass_data[i][1] = 0;
        getline(MASS, line);
        istringstream read(line);
        while (read >> check) {
            read.seekg(0, ios::beg);
            read >> mass_data[i][0];
   
 
 if (mass_data[i][0] < 0){
 freopen ("/dev/tty", "a", stdout);
        printf("Error: Invalid DM mass in model.in file.\n");
        exit(1);
       }

            read >> mass_data[i][1];


 if (mass_data[i][1] < 0){
 freopen ("/dev/tty", "a", stdout);
        printf("Error: Invalid Integrated Photon Spectrum in model.in file.\n");
        exit(1);
       }

            break;
        }
    }
    MASS.close();

    //check if "Output" directory exists, if does not, create "Output" directory
    struct stat st = {
        0
    };
    if (stat("Output", & st) == -1) {
        mkdir("Output", 0700);
    }

    string name;
    string modelname;
    int Juse = 1; //Variable to know if there are J factors
    int Jerror = 1; //Variable to know if there are J factors have error
    char set[10]; //Input for which set to use
    char setJ[10]; //Input to know if there are J factors
    char setJE[10]; //Input to know if J factors have error
    string outst = "Output/"; //create output string
    name = dat;
    modelname = in ;

    string dwset = "Input/";
    dwset.append(name);

    boost::erase_all(modelname, "/");
    boost::erase_all(modelname, ".in");
    boost::erase_all(modelname, "Input");
    boost::erase_all(name, "/");
    boost::erase_all(name, "Input");

    outst += modelname; //Add model name
    outst += "_";
    outst += name; //Add set name
    outst = outst.substr(0, outst.size() - 4); //remove set file extension
    outst += "_"; //Add underscore
    outst = outst + boost::lexical_cast < std::string > (boost::format("%.4f") % Beta); //Add confidence
   // outst += "_"; //Add underscore
   // time_t timer;
   // outst = outst + boost::lexical_cast < std::string > (boost::format("%.4f") % time( & timer)); //Add confidence
    outst += ".out"; //Add .out
    cout << "Working … ";
    cout << "File ";
    cout << outst;
    cout << " will be created shortly." << endl;

    const char * out = outst.c_str(); //setting the output string to the file name
    stdout = freopen(out, "w", stdout); // Setting the the print to the output file

    INPUT.open(dwset);
    if (!INPUT) {
freopen ("/dev/tty", "a", stdout);
        printf("Error: Failed to load dwarf data file.\n");
        exit(1);
    }

    string address = "Input"; //copies the dwarfset data into output
    address += "/";
    address += name;
    ifstream useddwarfset(address); //address is the location of dwarfset.dat file

    int counter = 0; //counter counts the number of the lines in the header of the dwarfset.dat file
    string linef;
    useddwarfset.seekg(0, ios::beg);
    while (getline(useddwarfset, linef)) {
        if (contains(linef, skip)) {
            counter++;
            cout << linef << endl;

        } else {
            break;
        }
    }

    int numlines = 0; //numline counts the number of the total lines of the dwarfset.dat file
    useddwarfset.seekg(0, ios::beg);
    while (getline(useddwarfset, linef))
        numlines++;
    useddwarfset.clear();

    //we add "#" at the beginning of all the lines of dwarfset.dat file after the header, from line number (counter+1) until line (numline)
    for (int j = 1; j < numlines - counter + 1; ++j) {
        useddwarfset.seekg(0, ios::beg);
        for (int i = 0; i < counter + j; ++i) {
            getline(useddwarfset, linef);

        }
        string linef2("#");
        linef2 += linef;
        cout << linef2 << endl;
    }
    cout << "#####################################################################################################################################################################" << endl;
    cout << "# Fermi_Pass8R3_239557417_585481831" << endl;
    cout << "###################################################################################OUTPUT############################################################################" << endl;

    float mass = mass_data[0][0];
    float energy = mass_data[0][1];
    float Nbound = process(Beta, INPUT, mass, energy, Juse, Jerror, 3); //Getting Nbound from the calculation
    for (int loop = 1; loop < line_count; ++loop) {
        INPUT.open(dwset);
        float mass = mass_data[loop][0]; //Changing the mass
        float energy = mass_data[loop][1]; //Changing the energy
        output(Nbound, Beta, INPUT, mass, energy, Juse, Jerror, 3); //Running just the output calculations

    }

    return 0;

}

/////////////////////////////////////////////////////////////////////////////////////////////////
int input(char * dat, float Beta, float mass, float energy) {
    //Telling it about your input file
    int Juse = 1; //Variable to know if there are J factors
    int Jerror = 1; //Variable to know if there are J factors have error
    char set[10]; //Input for which set to use
    char setJ[10]; //Input to know if there are J factors
    char setJE[10]; //Input to know if J factors have error

    string name;
    name = dat;
    string dwset = "Input/";
    dwset.append(name);

    ifstream INPUT;
    INPUT.open(dwset);
    if (!INPUT) {
freopen ("/dev/tty", "a", stdout);
        printf("Error: Failed to load dwarf data file.\n");
        exit(1);
    }
    float crosscons = 8 * M_PI * mass * mass / energy; //Creates a constant that is used to calculate the cross section

    if (fileout == true) {

        //check if "Output" directory exists, if does not, create "Output" directory
        struct stat st = {
            0
        };
        if (stat("Output", & st) == -1) {
            mkdir("Output", 0700);
        }

        string outst = "Output/"; //create output string
        outst += name; //Add set name
        outst = outst.substr(0, outst.size() - 4); //remove set file extension
        outst += "_"; //Add underscore
        outst = outst + boost::lexical_cast < std::string > (boost::format("%.4f") % Beta); //Add confidence
       // outst += "_"; //Add underscore
       // time_t timer;
      //  outst = outst + boost::lexical_cast < std::string > (boost::format("%.4f") % time( & timer)); //Add confidence
        outst += ".out"; //Add .out

        cout << "Working … ";
        cout << "File ";
        cout << outst;
        cout << " will be created shortly." << endl;

        const char * out = outst.c_str(); //setting the output string to the file name
        stdout = freopen(out, "w", stdout); // Setting the the print to the output file

        string address = "Input"; //copies the dwarfset data into output
        address += "/";
        address += name;
        ifstream useddwarfset(address); //address is the location of dwarfset.dat file

        int counter = 0; //counter counts the number of the lines in the header of the dwarfset.dat file
        string skip("#");
        string linef;
        useddwarfset.seekg(0, ios::beg);
        while (getline(useddwarfset, linef)) {
            if (contains(linef, skip)) {
                counter++;
                cout << linef << endl;

            } else {
                break;
            }
        }

        int numlines = 0; //numline counts the number of the total lines of the dwarfset.dat file
        useddwarfset.seekg(0, ios::beg);
        while (getline(useddwarfset, linef))
            numlines++;
        useddwarfset.clear();

        //we add "#" at the beginning of all the lines of dwarfset.dat file after the header, from line number (counter+1) until line (numline)
        for (int j = 1; j < numlines - counter + 1; ++j) {
            useddwarfset.seekg(0, ios::beg);
            for (int i = 0; i < counter + j; ++i) {
                getline(useddwarfset, linef);

            }
            string linef2("#");
            linef2 += linef;
            cout << linef2 << endl;
        }
        cout << "#####################################################################################################################################################################" << endl;
        cout << "# Fermi_Pass8R3_239557417_585481831" << endl;
        cout << "###################################################################################OUTPUT############################################################################" << endl;

    }

    process(Beta, INPUT, mass, energy, Juse, Jerror, 4);
    return 0;
}

/////////////////////////////////////////////////////////////
int main(int argc, char * argv[]) {

    ifstream flags("Input/flags.dat");
    int header = 0;
    string skip("#");
    string linef;
    string fileoutvalue;
    string printbetavalue;
    flags.seekg(0, ios::beg);
    while (getline(flags, linef)) {
        if (contains(linef, skip)) {
            header++;

        } else {
            break;
        }
    }

    flags.seekg(0, ios::beg);
    for (int i = 0; i < header + 1; ++i) {
        getline(flags, linef);

    }

    string word1;
    stringstream s1(linef);
    for (int i = 0; i < 2; i++) {
        s1 >> word1;
    }
    fileoutvalue.assign(word1);

    flags.seekg(0, ios::beg);
    for (int i = 0; i < header + 2; ++i) {
        getline(flags, linef);

    }

    string word2;
    stringstream s2(linef);
    for (int i = 0; i < 2; i++) {
        s2 >> word2;
    }
    printbetavalue.assign(word2);

    flags.clear();
    flags.close();

    if (fileoutvalue == "false")
        fileout = false;
    else
        fileout = true;

    if (printbetavalue == "false")
        printbeta = false;
    else
        printbeta = true;

    cout << "************************************************************" << endl;
    cout << "*                                                          *" << endl;
    cout << "*   MADHAT: Model-Agnostic Dark Halo Analysis Tool (v1.0)  *" << endl;
    cout << "*                                                          *" << endl;
    cout << "************************************************************" << endl;
   /* if (argc == 1 || argc == 2) {
        input(); //Run The Main Function
    }*/


    if (argc == 1 || argc == 2 || argc > 5) {
        freopen ("/dev/tty", "a", stdout);
        printf("Error: MADHAT requires 2, 3, or 4 arguments to run. \n For example, \"./madhat Set1.dat 0.95\". \n Please see the README for run options and argument formatting.\n");
        exit(1);
    }



  if (argc == 3) {
        float Beta = atof(argv[2]);
        if (Beta > 0 && Beta < 1) {
            input(argv[1], Beta);
        } else {
            cout << "Invalid Beta Value" << endl;
        }
    }
    if (argc == 4) {
        float Beta = atof(argv[2]);
        if (Beta > 0 && Beta < 1) {
            input(argv[1], Beta, argv[3]);

        } else {
            cout << "Invalid Beta Value" << endl;
        }
    }






 if (argc == 5) {

        float Beta = atof(argv[2]);
        float mass = atof(argv[3]);
        float energy = atof(argv[4]);

        if (mass > 0) {

                if (energy > 0) {
                               if (Beta > 0 && Beta < 1) { input(argv[1], Beta, mass, energy);}
                               else { cout << "Invalid Beta Value" << endl;}
                                }

                else {cout << "Invalid Integrated Photon Spectrum" << endl;}
                      }
         else {cout << "Invalid Mass Value" << endl;}
              }



    return 0; //End The Program
}
