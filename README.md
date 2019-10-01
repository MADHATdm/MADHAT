# MADHAT
Model-Agnostic Dark Halo Analysis Tool, version 1.0

MADHAT is a numerical tool that implements a Fermi-LAT data-driven and model-independent analysis of gamma-ray emission due to dark matter annihilation/decay in dwarf spheroidal galaxies.

Details of the analysis are described in https://arxiv.org/pdf/1802.03826.pdf. MADHAT allows a model-agnositic analysis in the sense a user can independently choose a set of dwarfs to analyze, a J factor (and errors) for each dwarf, and, if relevant, the dark matter mass and integrated photon energy spectrum.  Outputs include an upper limit (at a specified confidence level) on the number of photons that could have come from non-standard astrophysics, eg. dark matter annihilation or decay, and, if the dark matter mass and integrated energy spectrum are specified, an upper limit on the annihilation cross section.

This README is structured as follows:\
Required Libraries\
Make Instructions\
Running MADHAT\
Output Options and Formatting\
Input File Formatting\
Testing Your Installation

## Required Libraries 

MADHAT is dependent on [Boost](https://www.boost.org/) libaries. 

Documentation for [Boost version 1_70_0](https://www.boost.org/doc/libs/1_70_0/)

#### Boost Installation Instructions:

Brief instructions for Linux, Mac OS, and Windows are below, and full installation instructions for Boost can be found [here](https://www.boost.org/doc/libs/1_70_0/more/getting_started/index.html).

Once the installation is complete, you must edit line 5 of the MADHAT Makefile so that it specifies the correct path and version for your Boost installation, eg.
```
BOOST = [Path to Boost]/boost_1_70_0
``` 

**Windows:** Download [boost_1_70_0.zip](https://www.boost.org/users/history/version_1_70_0.html) or [boost_1_70_0.zip](https://www.boost.org/users/history/version_1_70_0.html) and unpack it to install a complete Boost distribution

**Linux and Mac OS:** Download [boost_1_70_0.tar.bz2](https://www.boost.org/users/history/version_1_70_0.html)
In the directory where you want to put the Boost installation, execute
```
tar --bzip2 -xf /path/to/boost_1_70_0.tar.bz2
```

## Make Instructions

To make the executable, run the command "make" in the main MADHAT directory.\
Note: to remove old files, run the command "make clean" in the main MADHAT directory.


## Running MADHAT 

To run MADHAT with command line input, type the command:
```
./madhat [options]
```
where [options] are described below.


### Run Options

There are three run options.  For Input File Formatting and Output Options and Formatting, see the relevant sections below.

**Run Option A) Specify the set of dwarfs to be analyzed, the confidence level (beta) for Nbound, and the dark matter model parameters.**  This option requires three arguments to run:
```
./madhat [dwarfset.dat] [beta] [model.in]
```
 * [dwarfset.dat] is the file containing the parameters for the dwarfs you'd like to analyze and must be located in the directory `Input`.
 * [beta] is a number between zero and one that specifies the confidence level (eg. 0.95 for 95% CL).
 * [model.in] is a file containing a list of dark matter masses and integrated photon spectra as described below and must be located in the directory `Input`. MADHAT will read [model.in] and calculate output for each line until it reaches the end of the [model.in] file.  
 
Output prints to a file in the `Output` directory named [model_dwarfset_beta.out].  The data from [dwarfset.dat] is copied into the header of the output file, below which are the following columns: mass, integrated photon spectrum, beta, Nbound(beta), PhiPP, +dPhiPP, -dPhiPP.

**Run Option B) Specify the set of dwarfs to be analyzed, the confidence level (β) for Nbound, and the mass and integrated photon spectrum for single dark matter model point:**
```
./madhat [dwarfset.dat] [beta] [mass] [integrated spectrum]
```
This option is a duplicate of option A, but allows the user to quickly check a single model point. Output will print to the screen.

**Run Option C) Specify the set of dwarfs to be analyzed and the confidence level (beta) for Nbound.**  This option requires two arguments to run:
```
./madhat [dwarfset.dat] [beta]
```
 * [dwarfset.dat] is the file containing the parameters for the dwarfs you'd like to analyze and must be located in the directory `Input`.
 * [beta] is a number between zero and one that specifies the confidence level (eg. 0.95 for 95% CL). 

Output prints to the screen in the following format: beta, Nbound(beta), PhiPP, +dPhiPP, -dPhiPP.


## Output Options and Formatting

For parameter definitions, see https://arxiv.org/pdf/1802.03826.pdf.

 * PhiPP [cm^3 s^-1 GeV^-2]
 * +/-dPhiPP [cm^3 s^-1 GeV^-2] (uncertainty due to varying J-factor)
 * sigv [cm^3 s^-1]
 * +/-dsigv [cm^3 s^-1] (uncertainty due to varying J-factor)

Note: The cross section limit calculation assumes that the dark matter particles are their own antiparticles. If the dark matter is Dirac rather than Majorana, divide the cross section by two. If the particle has some asymmetry between the particle and antiparticle, then the prefactor needs to be calculated. 

### Output Options
There are two optional output flags located in the file flags.dat, both of which default to "false". 

1. bool "fileout", on line 9 of flags.dat: If this is set to "true" then all run options print to a file []_[beta].out in the Output folder rather than to the screen.

2. bool "printbeta", on line 10
 of flags.dat: If this is set to "true" then for running with two or zero arguments (run options B and C), the output will be a table of Nbound(beta) for Nbound increasing as an integer up to the value [beta] passed as input.  Note that this output option is not defined for running with three inputs (run option A). Each line of the output will either have the format "beta, Nbound(beta), PhiPP, +dPhiPP, -dPhiPP, sigv, +dsigv, -dsigv" or "beta, Nbound(beta), PhiPP, +dPhiPP, -dPhiPP."  To output all values of (beta, Nbound), use [beta]=0.999 as input. 


## Input File Formatting

### Dwarf Sets

PMFs for 58 dwarfs and dwarf candidates determined with Fermi-LAT Pass 8R3 are included in the MADHAT 1.0 distribution.  There are ten pre-defined dwarf sets, including selected dwarfs and J-factors for each, located in the directory "Input:" \
Set1.dat [ref](https://doi.org/10.3847/1538-4357/834/2/110)\
Set1a.dat\
Set1b.dat\
Set1c.dat\
Set2.dat [ref](https://doi.org/10.1103/PhysRevD.93.103512)\
Set3.dat [ref](https://academic.oup.com/mnras/article-abstract/461/3/2914/2608532?redirectedFrom=fulltext)\
Set4.dat [ref](https://academic.oup.com/mnras/article-abstract/468/3/2884/3077187?redirectedFrom=fulltext), [ref](https://academic.oup.com/mnras/article-abstract/479/1/64/5017795?redirectedFrom=fulltext) \
Set5.dat [ref](https://dx.doi.org/10.1103/PhysRevD.95.123008)\
Set6.dat [ref](https://academic.oup.com/mnras/article-abstract/482/3/3480/5142321?redirectedFrom=fulltext) \
Set7.dat [ref](https://dx.doi.org/10.1088/1475-7516/2018/12/039)

To define a new dwarf set or enter new J-factor values, use the template SetTemplate.dat, which is a space-separated table of the following form:
```<pre>
############################################################
# MADHAT (Model-Agnostic Dark Halo Analysis Tool) 
# Dwarf Set: Template
#
# ID: Identify the Dwarf Galaxy to be analyzed
# J: Log10 of the J-factor, for J-factors specified in units of [GeV^2 cm^-5]
# +dJ: positive error in Log10(J)
# -dJ: negative error in Log10(J)
###########################################################
#   ID    J       +dJ    -dJ
    9     21.59   0.26   0.29
    10    21.52   0.26   0.29
    32    21.67   0.33   0.30
    36    22.25   0.37   0.62
    46    21.69   0.27   0.34
```
where ID indicates the dwarf ID number (see Dwarf ID Numbers below), J is the Log10 of the J-factor, for J-factors specified in units of [GeV^2 cm^-5], and +/-dJ is the error in Log10(J).  For example, for dwarf ID 9:\
Log10(J)=21.59\
Log10(J+dJ)=21.59+0.26\
Log10(J-dJ)=21.59-0.29

Please reserve filenames SetN.dat, where N are integers, for dwarf sets defined in future versions of MADHAT.

### Dark Matter Mass and Integrated Photon Spectrum

The input file model.in should have the following format:
```<pre>
############################################################
# MADHAT (Model-Agnostic Dark Halo Analysis Tool) 
# Dark Matter Model File: dmtest
#
# mass: DM mass in GeV
# spectrum: photon energy spectrum integrated from 1 to 100 GeV
###########################################################
# mass (GeV)   spectrum (1-100 GeV)
10      0.8734
50      7.7765
100     13.6097
500     33.7307
```
where the mass is a floating point number that specifies the dark matter mass in GeV and energy spectrum (1-100 GeV) is a floating point number that specifies the integrated photon energy spectrum between 1 and 100 GeV.  This file can be of arbitraty length.

### Dwarf ID Numbers

 1--Aquarius II\
 2--Bootes I\
 3--Bootes II\
 4--Bootes III\
 5--Canes Venatici I\
 6--Canes Venatici II\
 7--Carina\
 8--Carina II\
 9--Cetus II\
 10--Columba I\
 11--Coma Berenices\
 12--Draco\
 13--Draco II\
 14--Eridanus II\
 15--Eridanus III\
 16--Fornax\
 17--Grus I\
 18--Grus II\
 19--Hercules\
 20--Horologium I\
 21--Horologium II\
 22--Hydra II\
 23--Indus II\
 24--Kim 2\
 25--Leo I\
 26--Leo II\
 27--Leo IV\
 28--Leo T\
 29--Leo V\
 30--Pegasus III\
 31--Phoenix II\
 32--Pictor I\
 33--Pisces II\
 34--Reticulum II\
 35--Reticulum III\
 36--Sagittarius II\
 37--Sculptor\
 38--Segue 1\
 39--Segue 2\
 40--Sextans\
 41--Triangulum II\
 42--Tucana II\
 43--Tucana III\
 44--Tucana IV\
 45--Tucana V\
 46--Ursa Major I\
 47--Ursa Major II\
 48--Ursa Minor\
 49--Willman 1

## Testing Your Installation

The input
```
./madhat Set1.dat 0.95 dmtest.in
```
should produce an output file Output/dmtest_Set1_0.9500.out that contains the following:
```
#Mass(GeV)   Spectrum       Beta      Nbound        PhiPP(cm^3 s^-1 GeV^-2)   +dPhiPP             -dPhiPP            sigv(cm^3 s^-1)        +dsigv             -dsigv
10.0000     0.8734      0.9501     414.5011      3.728e-30            6.75934e-30        2.56887e-30        1.07276e-26        1.94505e-26        7.39213e-27
50.0000     7.7765      0.9500     414.5011      3.728e-30            6.75934e-30        2.56887e-30        3.01212e-26        5.46135e-26        2.07557e-26
100.0000     13.6097      0.9500     414.5011      3.728e-30            6.75934e-30        2.56887e-30        6.88443e-26        1.24823e-25        4.74388e-26
500.0000     33.7307      0.9500     414.5011      3.728e-30            6.75934e-30        2.56887e-30        6.94434e-25        1.2591e-24        4.78517e-25
```
