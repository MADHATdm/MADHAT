# MADHAT
Model-Agnostic Dark Halo Analysis Tool, version 1.0
Inline-style: 
![alt text](https://github.com/pearlsandick/MADHAT/blob/master/MADHATlogo.png "MADHAT logo")

MADHAT is a numerical tool that implements a Fermi-LAT data-driven and model-independent analysis of gamma-ray emission due to dark matter annihilation/decay in dwarf spheroidal galaxies.

If you use MADHAT, please cite the following papers:\
[1] K. Boddy, D. Kumar, D. Marfatia, and P. Sandick, Phys.Rev. D97 (2018) no.9, 095031, https://arxiv.org/abs/1802.03826\
[2] K. Boddy, S. Hill, J. Kumar, P. Sandick, and B. Shams Es Haghi, in prep.


MADHAT allows a model-agnositic analysis in the sense a user can independently choose a set of dwarfs and dwarf candidates to analyze, a J factor (and errors) for each object, and, if relevant, the dark matter mass and integrated photon energy spectrum.  Outputs include an upper limit (at a specified confidence level) on the number of photons that could have come from non-standard astrophysics, eg. dark matter annihilation or decay, and, if the dark matter mass and integrated energy spectrum are specified, an upper limit on the annihilation cross section.

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

Note: The cross section limit calculation assumes that the dark matter particles are their own antiparticles. If the dark matter is Dirac rather than Majorana, multiply the cross section by two. If the particle has some asymmetry between the particle and antiparticle, then the prefactor needs to be calculated. 

### Output Options
There are two optional output flags located in the file flags.dat, both of which default to "false". 

1. bool "fileout", on line 9 of flags.dat: If this is set to "true" then all run options print to a file []_[beta].out in the Output folder rather than to the screen.

2. bool "printbeta", on line 10
 of flags.dat: If this is set to "true" then for running with two or zero arguments (run options B and C), the output will be a table of Nbound(beta) for Nbound increasing as an integer up to the value [beta] passed as input.  Note that this output option is not defined for running with three inputs (run option A). Each line of the output will either have the format "beta, Nbound(beta), PhiPP, +dPhiPP, -dPhiPP, sigv, +dsigv, -dsigv" or "beta, Nbound(beta), PhiPP, +dPhiPP, -dPhiPP."  To output all values of (beta, Nbound), use [beta]=0.999 as input. 


## Input File Formatting

### Dwarf Sets

PMFs for 58 dwarfs and dwarf candidates determined with Fermi-LAT Pass 8R3 are included in the MADHAT 1.0 distribution.  There are ten pre-defined dwarf sets, including selected dwarfs and J-factors for each, located in the directory `Input`: \
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
# ID    J       +dJ    -dJ
6	    20.76    	.24   	.30
10	   21.51	    .31	   .24
14	   20.99    1.08	   .37
23.  	20.57	    .36	   .21
24	   20.58    	.48   	.35
35.  	21.37    	.34	   .28
38	   20.54   	2.15	   .39
46	   21.63	    .44	   .28
```
where ID indicates the dwarf ID number (see Dwarf ID Numbers below), J is the Log10 of the J-factor, for J-factors specified in units of [GeV^2 cm^-5], and +/-dJ is the error in Log10(J).  For example, for dwarf ID 10:\
Log10(J)=21.51\
Log10(J+dJ)=21.51+0.31\
Log10(J-dJ)=21.51-0.24

A dwarf set file must contain, at a minimum, the ID numbers for the dwarfs to be considered.  It may also contain J-factors for each dwarf and J-factor errors.  Note that errors are not necessary, and if omitted, errors on PhiPP will print as zeros. Similarly, if the file only contains dwarf ID numbers, PhiPP will also print as zero. There are ten dwarfset.dat files included in the MADHAT package, plus a template file, SetTemplate.dat.

Note that all input files must end with a line of data (no additional newlines). Please reserve filenames SetN.dat, where N are integers, for dwarf sets defined in future versions of MADHAT.

### Dark Matter Mass and Integrated Photon Spectrum

The input file model.in must be located in the `Input` directory, and should have the following format:
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
500     33.6607
```
where the mass is a floating point number that specifies the dark matter mass in GeV and energy spectrum (1-100 GeV) is a floating point number that specifies the integrated photon energy spectrum between 1 and 100 GeV.  This file can be of arbitraty length, and must end with a line of data (no additional newlines).  The MADHAT package includes the dmtest.in template file, as well as four dark matter model files for two-body annihilation to b quarks, W bosons, muon pairs, and tau pairs.

### Dwarf ID Numbers

1--Aquarius II\
 2--Bootes I\
 3--Bootes II\
 4--Bootes III\
 5--Canes Venatici I\
 6--Canes Venatici II\
 7--Canis Major\
 8--Carina\
 9--Carina II\
 10--Carina III\
 11--Cetus II\
 12--Cetus III\
 13--Columba I\
 14--Coma Berenices\
 15--Crater II\
 16--Draco\
 17--Draco II\
 18--Eridanus II\
 19--Eridanus III\
 20--Fornax\
 21--Grus I\
 22--Grus II\
 23--Hercules\
 24--Horologium I\
 25--Horologium II\
 26--Hydra II\
 27--Hydrus I\
 28--Indus II\
 29--Kim 2\
 30--Laevens 3\
 31--Leo I\
 32--Leo II\
 33--Leo IV\
 34--Leo T\
 35--Leo V\
 36--Pegasus III\
 37--Phoenix II\
 38--Pictor I\
 39--Pictor II\
 40--Pisces II\
 41--Reticulum II\
 42--Reticulum III\
 43--Sagittarius\
 44--Sagittarius II\
 45--Sculptor\
 46--Segue 1\
 47--Segue 2\
 48--Sextans\
 49--Triangulum II\
 50--Tucana II\
 51--Tucana III\
 52--Tucana IV\
 53--Tucana V\
 54--Ursa Major I\
 55--Ursa Major II\
 56--Ursa Minor\
 57--Virgo I\
 58--Willman 1

## Testing Your Installation

The input
```
./madhat Set1.dat 0.95 dmtest.in
```
should produce an output file Output/dmtest_Set1_0.9500.out that contains the following:
```

```
