#!/usr/bin/perl

########################################################################
# Copyright (C) 2007 VMWare, Inc. All rights reserved.
# -- VMware Confidential
########################################################################

## command.pl : Launcher script to execute array specific scripts.
 
# input: 
#        XML Input from STDIN. XML input formats are described in DTD 
#        specification of the XML interface for interacting with the Array
#        Integration scripts:
#        https://wiki.eng.vmware.com/ArrayManagement/VIMDisasterRecovery/DTDSpecification
## @todo the DTD URL will need to be changed.
# 
#        Relevant Element descriptions as follows:
#        Command::Name - operation to perform (discoverArrays, etc) 
#        Command::OutputFile - file to write XML output data 
#        Command::LogLevel - optional log level control, default is info.
# 
#        Example:
#        <?xml version="1.0" encoding="ISO-8859-1"?>
#        <Command>
#           <Name>discoverArrays</Name>
#           <OutputFile>discoverArrays.xml</OutputFile>
#           ...
#           <LogLevel>info</LogLevel>
#        </Command>
# 
# output:
#        XML output written to file specified by <OutputFile>.
#        XML output formats are described in DTD specification of the XML
#        interface for interacting with the Array Integration scripts: 
#        https://wiki.eng.vmware.com/ArrayManagement/VIMDisasterRecovery/DTDSpecification
## @todo the DTD URL will need to be changed.
#        
#        Debug/log/CLI output/CLI error written to STDOUT.
# 
# return: 
#     Exit Code: UNKNOWNERROR if OutputFile is NULL or missing in Input XML,
#                or failure to write to XML output.
#                SUCCESS, otherwise.
#                Note: other error such as Command::Name not found is reported
#                      in XML output ReturnCode.
# notes:
#     This script is the entry point for all commands accepted by the 
#     array manager. The commands are:
#        discoverArrays: discovery of arrays.  
#        discoverLuns: discovery of replicated LUNs. 
#        failover: failover of LUNs. 
#        testFailover (Start): creates a volume containing replicated data on 
#           a secondary snapshot and ensures host visibility to it. 
#        testFailover (Stop): terminates/cleanup the failover test.
# 

use XML::Simple qw(:strict);
use Data::Dumper;
use strict;
use FindBin qw($Bin);   # Locate directory of perl script
use lib "$Bin";

my $path = $Bin; # directory of the script to be called 

if(@ARGV >= 1)
{
   commandUsage();
   exit(1);
}

#Get command input
my $xml = new XML::Simple (KeyAttr => [], ForceArray => 1, KeepRoot=>1);
my $ref = eval{$xml->XMLin('-')};   # read XML input data from STDIN

my $logLevelString = $ref->{Command}[0]->{LogLevel}[0];
my $commandName = $ref->{Command}[0]->{Name}[0];
my $outputFile = $ref->{Command}[0]->{OutputFile}[0];

print "name: $commandName \n";
print "log level: $logLevelString \n";
print "output file: $outputFile \n";

open(my $dataInput, "| \"$Bin\/sra.exe\"");
my $xmlOutput = eval{$xml->XMLout($ref, OutputFile => $dataInput)};
close($dataInput);

exit(0);

sub commandUsage{

    print <<USAGE_EOF;
3PAR Site Recovery Agent command:

Syntax:
   perl command.pl < inputXMLFile
			
   Currently supported commands are: 
   discoverArrays,discoverLuns,failover, and testFailover.
        
USAGE_EOF

}