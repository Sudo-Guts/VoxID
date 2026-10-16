##
## Altair PSIM - Python script for  "Filtro_TL072.psimsch" 
##



from psimapipy import*
from pathlib import Path
import os

# Get the directory of the current script
current_script_dir = os.path.dirname(os.path.realpath(__file__))

# Get the script's file name
script_file_name = os.path.basename(__file__)
script_name, ext = os.path.splitext(script_file_name)

schematic_filePath = current_script_dir + "\\" + script_name + ".psimsch"
#print(schematic_filePath)
str_save_FolderPath = current_script_dir + "\\" 
print("Loading PSIM...");
p1 = PSIM("")
if (p1 and p1.IsValid()) :
	ver, subVer, subsubver, subsubsubver = p1.get_psim_version() 



	sch = p1.PsimFileNew()
	if (sch) : 

		# Simulation Control settings:
		p1.PsimSetElmValue(sch, None, "TIMESTEP", "1E-05") 
		p1.PsimSetElmValue(sch, None, "TOTALTIME", "0.01") 
		p1.PsimSetElmValue(sch, None, "PRINTTIME", "0") 
		p1.PsimSetElmValue(sch, None, "PRINTSTEP", "1") 
		p1.PsimSetElmValue(sch, None, "SAVEFLAG", "0") 
		p1.PsimSetElmValue(sch, None, "LOADFLAG", "0") 
		p1.PsimSetElmValue(sch, None, "RSWITCHON", "1E-05") 
		p1.PsimSetElmValue(sch, None, "RSWITCHOFF", "10000000") 

		# Spice specific settings:
		p1.PsimSetElmValue(sch, None, "SPICETIMESTEP", "1.0e-006") 
		p1.PsimSetElmValue(sch, None, "MAXSTEP", "1.0e-006") 
		p1.PsimSetElmValue(sch, None, "STARTTIME", "0") 
		p1.PsimSetElmValue(sch, None, "ENDTIME", "1.0e-003") 

		# Add Simulation Control  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "SIMCONTROL", "", DIRECTION = 0, AREA = [248, 153, 288, 193], PAGE=0, XFLIP=0, _OPTIONS_=0) 

		# Add a Capacitor  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "MULTI_CAPACITOR", "C1", SubType="Level 1"  , AREA = [330, 270, 380, 290], DIRECTION = 0, PAGE=0, XFLIP=0, _OPTIONS_=16 , PORTS=[330, 280, 380, 280] , Capacitance = "160n" , Current_Flag = "0" , info_Manufacturer = "" , info_Package = "" , info_Part_No_ = "" , info_Voltage_Rating = "" , Init__Cap__Voltage = "0" , Voltage_Flag = "0" ) 
		p1.PsimCreateNewElement(sch, "TEXTLINK", "", AREA = [355, 262, 397, 277], DIRECTION = 0, PAGE=0, XFLIP=0 , _OPTIONS_=0 , OWNERINDEX=nCreatedIndex, FieldIndex= "1", FieldName= "Capacitance", FontName="Arial", FontSize=-15, Color="0", FontOptions=65936, FormatFactor=21.475464 ) 

		# Add a Capacitor  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "MULTI_CAPACITOR", "C2", SubType="Level 1"  , AREA = [400, 270, 450, 290], DIRECTION = 0, PAGE=0, XFLIP=0, _OPTIONS_=16 , PORTS=[400, 280, 450, 280] , Capacitance = "160n" , Current_Flag = "0" , info_Manufacturer = "" , info_Package = "" , info_Part_No_ = "" , info_Voltage_Rating = "" , Init__Cap__Voltage = "0" , Voltage_Flag = "0" ) 
		p1.PsimCreateNewElement(sch, "TEXTLINK", "", AREA = [425, 262, 467, 277], DIRECTION = 0, PAGE=0, XFLIP=0 , _OPTIONS_=0 , OWNERINDEX=nCreatedIndex, FieldIndex= "1", FieldName= "Capacitance", FontName="Arial", FontSize=-15, Color="0", FontOptions=65936, FormatFactor=21.475464 ) 

		# Add a Resistor  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "MULTI_RESISTOR", "R1", SubType="Level 1"  , AREA = [440, 230, 490, 250], DIRECTION = 0, PAGE=0, XFLIP=0, _OPTIONS_=16 , PORTS=[440, 240, 490, 240] , Current_Flag = "0" , info_Manufacturer = "" , info_Package = "" , info_Part_No_ = "" , info_Power_Rating = "1/4W" , Resistance = "3.81k" , Voltage_Flag = "0" ) 
		p1.PsimCreateNewElement(sch, "TEXTLINK", "", AREA = [469, 223, 512, 238], DIRECTION = 0, PAGE=0, XFLIP=0 , _OPTIONS_=0 , OWNERINDEX=nCreatedIndex, FieldIndex= "1", FieldName= "Resistance", FontName="Arial", FontSize=-15, Color="0", FontOptions=65936, FormatFactor=21.528503 ) 

		# Add a Resistor  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "MULTI_RESISTOR", "R2", SubType="Level 1"  , AREA = [450, 290, 470, 340], DIRECTION = 90, PAGE=0, XFLIP=0, _OPTIONS_=16 , PORTS=[460, 290, 460, 340] , Current_Flag = "0" , info_Manufacturer = "" , info_Package = "" , info_Part_No_ = "" , info_Power_Rating = "1/4W" , Resistance = "26k" , Voltage_Flag = "0" ) 
		p1.PsimCreateNewElement(sch, "TEXTLINK", "", AREA = [470, 307, 505, 322], DIRECTION = 0, PAGE=0, XFLIP=0 , _OPTIONS_=0 , OWNERINDEX=nCreatedIndex, FieldIndex= "1", FieldName= "Resistance", FontName="Arial", FontSize=-15, Color="0", FontOptions=65936, FormatFactor=17.675293 ) 

		# Add a Ground  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "Ground", "Ground" , AREA = [450, 350, 470, 370], DIRECTION = 0, PAGE=0, XFLIP=0, _OPTIONS_=16, PORTS=[460, 350] ) 

		# Add a Capacitor  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "MULTI_CAPACITOR", "C3", SubType="Level 1"  , AREA = [570, 280, 620, 300], DIRECTION = 0, PAGE=0, XFLIP=0, _OPTIONS_=16 , PORTS=[570, 290, 620, 290] , Capacitance = "160n" , Current_Flag = "0" , info_Manufacturer = "" , info_Package = "" , info_Part_No_ = "" , info_Voltage_Rating = "" , Init__Cap__Voltage = "0" , Voltage_Flag = "0" ) 
		p1.PsimCreateNewElement(sch, "TEXTLINK", "", AREA = [594, 272, 637, 287], DIRECTION = 0, PAGE=0, XFLIP=0 , _OPTIONS_=0 , OWNERINDEX=nCreatedIndex, FieldIndex= "1", FieldName= "Capacitance", FontName="Arial", FontSize=-15, Color="0", FontOptions=65936, FormatFactor=21.475464 ) 

		# Add a Capacitor  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "MULTI_CAPACITOR", "C4", SubType="Level 1"  , AREA = [640, 280, 690, 300], DIRECTION = 0, PAGE=0, XFLIP=0, _OPTIONS_=16 , PORTS=[640, 290, 690, 290] , Capacitance = "160n" , Current_Flag = "0" , info_Manufacturer = "" , info_Package = "" , info_Part_No_ = "" , info_Voltage_Rating = "" , Init__Cap__Voltage = "0" , Voltage_Flag = "0" ) 
		p1.PsimCreateNewElement(sch, "TEXTLINK", "", AREA = [664, 272, 707, 287], DIRECTION = 0, PAGE=0, XFLIP=0 , _OPTIONS_=0 , OWNERINDEX=nCreatedIndex, FieldIndex= "1", FieldName= "Capacitance", FontName="Arial", FontSize=-15, Color="0", FontOptions=65936, FormatFactor=21.475464 ) 

		# Add a Resistor  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "MULTI_RESISTOR", "R3", SubType="Level 1"  , AREA = [680, 240, 730, 260], DIRECTION = 0, PAGE=0, XFLIP=0, _OPTIONS_=16 , PORTS=[680, 250, 730, 250] , Current_Flag = "0" , info_Manufacturer = "" , info_Package = "" , info_Part_No_ = "" , info_Power_Rating = "1/4W" , Resistance = "9.2k" , Voltage_Flag = "0" ) 
		p1.PsimCreateNewElement(sch, "TEXTLINK", "", AREA = [709, 232, 745, 248], DIRECTION = 0, PAGE=0, XFLIP=0 , _OPTIONS_=0 , OWNERINDEX=nCreatedIndex, FieldIndex= "1", FieldName= "Resistance", FontName="Arial", FontSize=-15, Color="0", FontOptions=65936, FormatFactor=18.293945 ) 

		# Add a Resistor  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "MULTI_RESISTOR", "R4", SubType="Level 1"  , AREA = [690, 300, 710, 350], DIRECTION = 90, PAGE=0, XFLIP=0, _OPTIONS_=16 , PORTS=[700, 300, 700, 350] , Current_Flag = "0" , info_Manufacturer = "" , info_Package = "" , info_Part_No_ = "" , info_Power_Rating = "1/4W" , Resistance = "10.8k" , Voltage_Flag = "0" ) 
		p1.PsimCreateNewElement(sch, "TEXTLINK", "", AREA = [709, 317, 752, 332], DIRECTION = 0, PAGE=0, XFLIP=0 , _OPTIONS_=0 , OWNERINDEX=nCreatedIndex, FieldIndex= "1", FieldName= "Resistance", FontName="Arial", FontSize=-15, Color="0", FontOptions=65936, FormatFactor=21.528503 ) 

		# Add a Ground  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "Ground", "Ground" , AREA = [690, 360, 710, 380], DIRECTION = 0, PAGE=0, XFLIP=0, _OPTIONS_=16, PORTS=[700, 360] ) 

		# Add a Capacitor  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "MULTI_CAPACITOR", "C5", SubType="Level 1"  , AREA = [950, 310, 970, 360], DIRECTION = 90, PAGE=0, XFLIP=0, _OPTIONS_=16 , PORTS=[960, 310, 960, 360] , Capacitance = "2.7n" , Current_Flag = "0" , info_Manufacturer = "" , info_Package = "" , info_Part_No_ = "" , info_Voltage_Rating = "" , Init__Cap__Voltage = "0" , Voltage_Flag = "0" ) 
		p1.PsimCreateNewElement(sch, "TEXTLINK", "", AREA = [970, 327, 1008, 342], DIRECTION = 0, PAGE=0, XFLIP=0 , _OPTIONS_=0 , OWNERINDEX=nCreatedIndex, FieldIndex= "1", FieldName= "Capacitance", FontName="Arial", FontSize=-15, Color="0", FontOptions=65936, FormatFactor=19.089294 ) 

		# Add a Capacitor  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "MULTI_CAPACITOR", "C6", SubType="Level 1"  , AREA = [940, 250, 990, 270], DIRECTION = 0, PAGE=0, XFLIP=0, _OPTIONS_=16 , PORTS=[940, 260, 990, 260] , Capacitance = "27n" , Current_Flag = "0" , info_Manufacturer = "" , info_Package = "" , info_Part_No_ = "" , info_Voltage_Rating = "" , Init__Cap__Voltage = "0" , Voltage_Flag = "0" ) 
		p1.PsimCreateNewElement(sch, "TEXTLINK", "", AREA = [967, 241, 1004, 257], DIRECTION = 0, PAGE=0, XFLIP=0 , _OPTIONS_=0 , OWNERINDEX=nCreatedIndex, FieldIndex= "1", FieldName= "Capacitance", FontName="Arial", FontSize=-15, Color="0", FontOptions=65936, FormatFactor=18.559082 ) 

		# Add a Resistor  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "MULTI_RESISTOR", "R5", SubType="Level 1"  , AREA = [830, 290, 880, 310], DIRECTION = 0, PAGE=0, XFLIP=0, _OPTIONS_=16 , PORTS=[830, 300, 880, 300] , Current_Flag = "0" , info_Manufacturer = "" , info_Package = "" , info_Part_No_ = "" , info_Power_Rating = "1/4W" , Resistance = "1.64k" , Voltage_Flag = "0" ) 
		p1.PsimCreateNewElement(sch, "TEXTLINK", "", AREA = [861, 282, 904, 298], DIRECTION = 0, PAGE=0, XFLIP=0 , _OPTIONS_=0 , OWNERINDEX=nCreatedIndex, FieldIndex= "1", FieldName= "Resistance", FontName="Arial", FontSize=-15, Color="0", FontOptions=65936, FormatFactor=21.528503 ) 

		# Add a Resistor  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "MULTI_RESISTOR", "R6", SubType="Level 1"  , AREA = [900, 290, 950, 310], DIRECTION = 0, PAGE=0, XFLIP=0, _OPTIONS_=16 , PORTS=[900, 300, 950, 300] , Current_Flag = "0" , info_Manufacturer = "" , info_Package = "" , info_Part_No_ = "" , info_Power_Rating = "1/4W" , Resistance = "5.87k" , Voltage_Flag = "0" ) 
		p1.PsimCreateNewElement(sch, "TEXTLINK", "", AREA = [925, 282, 968, 297], DIRECTION = 0, PAGE=0, XFLIP=0 , _OPTIONS_=0 , OWNERINDEX=nCreatedIndex, FieldIndex= "1", FieldName= "Resistance", FontName="Arial", FontSize=-15, Color="0", FontOptions=65936, FormatFactor=21.528503 ) 

		# Add a Ground  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "Ground", "Ground" , AREA = [950, 370, 970, 390], DIRECTION = 0, PAGE=0, XFLIP=0, _OPTIONS_=16, PORTS=[960, 370] ) 

		# Add a Capacitor  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "MULTI_CAPACITOR", "C7", SubType="Level 1"  , AREA = [1190, 320, 1210, 370], DIRECTION = 90, PAGE=0, XFLIP=0, _OPTIONS_=16 , PORTS=[1200, 320, 1200, 370] , Capacitance = "20n" , Current_Flag = "0" , info_Manufacturer = "" , info_Package = "" , info_Part_No_ = "" , info_Voltage_Rating = "" , Init__Cap__Voltage = "0" , Voltage_Flag = "0" ) 
		p1.PsimCreateNewElement(sch, "TEXTLINK", "", AREA = [1214, 336, 1251, 351], DIRECTION = 0, PAGE=0, XFLIP=0 , _OPTIONS_=0 , OWNERINDEX=nCreatedIndex, FieldIndex= "1", FieldName= "Capacitance", FontName="Arial", FontSize=-15, Color="0", FontOptions=65936, FormatFactor=18.559082 ) 

		# Add a Capacitor  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "MULTI_CAPACITOR", "C8", SubType="Level 1"  , AREA = [1180, 260, 1230, 280], DIRECTION = 0, PAGE=0, XFLIP=0, _OPTIONS_=16 , PORTS=[1180, 270, 1230, 270] , Capacitance = "27n" , Current_Flag = "0" , info_Manufacturer = "" , info_Package = "" , info_Part_No_ = "" , info_Voltage_Rating = "" , Init__Cap__Voltage = "0" , Voltage_Flag = "0" ) 
		p1.PsimCreateNewElement(sch, "TEXTLINK", "", AREA = [1211, 250, 1248, 266], DIRECTION = 0, PAGE=0, XFLIP=0 , _OPTIONS_=0 , OWNERINDEX=nCreatedIndex, FieldIndex= "1", FieldName= "Capacitance", FontName="Arial", FontSize=-15, Color="0", FontOptions=65936, FormatFactor=18.559082 ) 

		# Add a Resistor  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "MULTI_RESISTOR", "R7", SubType="Level 1"  , AREA = [1070, 300, 1120, 320], DIRECTION = 0, PAGE=0, XFLIP=0, _OPTIONS_=16 , PORTS=[1070, 310, 1120, 310] , Current_Flag = "0" , info_Manufacturer = "" , info_Package = "" , info_Part_No_ = "" , info_Power_Rating = "1/4W" , Resistance = "779" , Voltage_Flag = "0" ) 
		p1.PsimCreateNewElement(sch, "TEXTLINK", "", AREA = [1106, 291, 1143, 307], DIRECTION = 0, PAGE=0, XFLIP=0 , _OPTIONS_=0 , OWNERINDEX=nCreatedIndex, FieldIndex= "1", FieldName= "Resistance", FontName="Arial", FontSize=-15, Color="0", FontOptions=65936, FormatFactor=18.559082 ) 

		# Add a Resistor  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "MULTI_RESISTOR", "R8", SubType="Level 1"  , AREA = [1140, 300, 1190, 320], DIRECTION = 0, PAGE=0, XFLIP=0, _OPTIONS_=16 , PORTS=[1140, 310, 1190, 310] , Current_Flag = "0" , info_Manufacturer = "" , info_Package = "" , info_Part_No_ = "" , info_Power_Rating = "1/4W" , Resistance = "1.67k" , Voltage_Flag = "0" ) 
		p1.PsimCreateNewElement(sch, "TEXTLINK", "", AREA = [1169, 291, 1212, 306], DIRECTION = 0, PAGE=0, XFLIP=0 , _OPTIONS_=0 , OWNERINDEX=nCreatedIndex, FieldIndex= "1", FieldName= "Resistance", FontName="Arial", FontSize=-15, Color="0", FontOptions=65936, FormatFactor=21.528442 ) 

		# Add a Ground  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "Ground", "Ground" , AREA = [1190, 380, 1210, 400], DIRECTION = 0, PAGE=0, XFLIP=0, _OPTIONS_=16, PORTS=[1200, 380] ) 

		# Add a AC sweep parameters  
		nCreatedIndex = p1.PsimCreateNewElement(sch, ".ACSWEEP", "ACSWEEP1" , AREA = [190, 220, 230, 260], DIRECTION = 0, PAGE=0, XFLIP=0, _OPTIONS_=16, PORTS=[] , End_Amplitude = "0.1" , End_Frequency = "50k" , Flag_for_Points = "0" , Freq__for_extra_Points = "" , No__of_Points = "50" , Source_Name = "Vsweep" , Start_Amplitude = "0.1" , Start_Frequency = "1" ) 
		p1.PsimCreateNewElement(sch, "TEXTLINK", "", AREA = [210, 252, 242, 267], DIRECTION = 0, PAGE=0, XFLIP=0 , _OPTIONS_=0 , OWNERINDEX=nCreatedIndex, FieldIndex= "3", FieldName= "No. of Points", FontName="Arial", FontSize=-15, Color="0", FontOptions=65936, FormatFactor=15.907761 ) 
		p1.PsimCreateNewElement(sch, "TEXTLINK", "", AREA = [210, 212, 238, 227], DIRECTION = 0, PAGE=0, XFLIP=0 , _OPTIONS_=0 , OWNERINDEX=nCreatedIndex, FieldIndex= "1", FieldName= "Start Frequency", FontName="Arial", FontSize=-15, Color="0", FontOptions=65936, FormatFactor=14.316986 ) 
		p1.PsimCreateNewElement(sch, "TEXTLINK", "", AREA = [209, 268, 272, 284], DIRECTION = 0, PAGE=0, XFLIP=0 , _OPTIONS_=0 , OWNERINDEX=nCreatedIndex, FieldIndex= "5", FieldName= "Source Name", FontName="Arial", FontSize=-15, Color="0", FontOptions=65936, FormatFactor=31.815521 ) 
		p1.PsimCreateNewElement(sch, "TEXTLINK", "", AREA = [209, 195, 245, 211], DIRECTION = 0, PAGE=0, XFLIP=0 , _OPTIONS_=0 , OWNERINDEX=nCreatedIndex, FieldIndex= "2", FieldName= "End Frequency", FontName="Arial", FontSize=-15, Color="0", FontOptions=65936, FormatFactor=17.675293 ) 

		# Add a Sinusoidal voltage source  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "VSIN", "Vsweep" , AREA = [300, 290, 320, 340], DIRECTION = 0, PAGE=0, XFLIP=0, _OPTIONS_=16, PORTS=[310, 290, 310, 340] , AC_Analysis = "0" , DC_Offset = "0" , Frequency = "" , Lseries = "0" , Peak_Amplitude = "1" , Phase_Angle = "0" , Rseries = "0" , Tstart = "0" ) 
		p1.PsimCreateNewElement(sch, "TEXTLINK", "", AREA = [320, 295, 348, 310], DIRECTION = 0, PAGE=0, XFLIP=0 , _OPTIONS_=0 , OWNERINDEX=nCreatedIndex, FieldIndex= "1", FieldName= "Peak Amplitude", FontName="Arial", FontSize=-15, Color="0", FontOptions=65936, FormatFactor=14.316986 ) 
		p1.PsimCreateNewElement(sch, "TEXTLINK", "", AREA = [236, 307, 299, 322], DIRECTION = 0, PAGE=0, XFLIP=0 , _OPTIONS_=0 , OWNERINDEX=nCreatedIndex, FieldIndex= "0", FieldName= "Name", FontName="Arial", FontSize=-15, Color="0", FontOptions=131472, FormatFactor=31.815521 ) 

		# Add a Ground  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "Ground", "Ground" , AREA = [300, 350, 320, 370], DIRECTION = 0, PAGE=0, XFLIP=0, _OPTIONS_=16, PORTS=[310, 350] ) 

		# Add a Voltage probe (node to ground)  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "VP", "V1" , AREA = [1290, 240, 1310, 270], DIRECTION = 0, PAGE=0, XFLIP=0, _OPTIONS_=16, PORTS=[1300, 270] , enable_flag = "1" ) 
		p1.PsimCreateNewElement(sch, "TEXTLINK", "", AREA = [1310, 247, 1346, 262], DIRECTION = 0, PAGE=0, XFLIP=0 , _OPTIONS_=0 , OWNERINDEX=nCreatedIndex, FieldIndex= "0", FieldName= "Name", FontName="Arial", FontSize=-15, Color="0", FontOptions=65936, FormatFactor=18.028809 ) 

		# Add a AC sweep probe   
		nCreatedIndex = p1.PsimCreateNewElement(sch, "ACSWEEP_OUT", "Vo1" , AREA = [1310, 310, 1340, 330], DIRECTION = 90, PAGE=0, XFLIP=0, _OPTIONS_=16, PORTS=[1310, 320] ) 

		# Add a SPICE Directive Block  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "SPICE_DIRECTIVE_BLOCK", "TL072" , Ref3Party=22 , AREA = [750, 90, 830, 150], DIRECTION = 0, PAGE=0, XFLIP=0, _OPTIONS_=16, PORTS=[] , _InputCount = "0" , _OutputCount = "0" ) 
		p1.PsimCreateNewElement(sch, "TEXTLINK", "", AREA = [765, 70, 814, 85], DIRECTION = 0, PAGE=0, XFLIP=0 , _OPTIONS_=0 , OWNERINDEX=nCreatedIndex, FieldIndex= "1000", FieldName= "Name", FontName="Arial", FontSize=-15, Color="FF0000", FontOptions=400, FormatFactor=24.497925 ) 

		# Add a SPICE Subcircuit Netlist Block  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "SPICE_SUB_NETLIST_BLOCK", "TL071" , Ref3Party=23 , AREA = [500, 270, 550, 310], DIRECTION = 0, PAGE=0, XFLIP=0, _OPTIONS_=16, PORTS=[500, 280, 500, 300, 530, 270, 530, 310, 550, 290] , _InputCount = "5" , _OutputCount = "0" ) 

		# Add a SPICE Subcircuit Netlist Block  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "SPICE_SUB_NETLIST_BLOCK", "TL072" , Ref3Party=23 , AREA = [740, 280, 790, 320], DIRECTION = 0, PAGE=0, XFLIP=0, _OPTIONS_=16, PORTS=[740, 290, 740, 310, 770, 280, 770, 320, 790, 300] , _InputCount = "5" , _OutputCount = "0" ) 

		# Add a SPICE Subcircuit Netlist Block  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "SPICE_SUB_NETLIST_BLOCK", "TL073" , Ref3Party=23 , AREA = [1000, 290, 1050, 330], DIRECTION = 0, PAGE=0, XFLIP=0, _OPTIONS_=16, PORTS=[1000, 300, 1000, 320, 1030, 290, 1030, 330, 1050, 310] , _InputCount = "5" , _OutputCount = "0" ) 

		# Add a SPICE Subcircuit Netlist Block  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "SPICE_SUB_NETLIST_BLOCK", "TL74" , Ref3Party=23 , AREA = [1240, 300, 1290, 340], DIRECTION = 0, PAGE=0, XFLIP=0, _OPTIONS_=16, PORTS=[1240, 310, 1240, 330, 1270, 300, 1270, 340, 1290, 320] , _InputCount = "5" , _OutputCount = "0" ) 

		# Add a DC voltage source  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "VDC", "VDC1" , AREA = [350, 400, 370, 450], DIRECTION = 0, PAGE=0, XFLIP=0, _OPTIONS_=16, PORTS=[360, 400, 360, 450] , Amplitude = "12" , Lseries = "0" , Rseries = "0" ) 
		p1.PsimCreateNewElement(sch, "TEXTLINK", "", AREA = [370, 417, 401, 432], DIRECTION = 0, PAGE=0, XFLIP=0 , _OPTIONS_=0 , OWNERINDEX=nCreatedIndex, FieldIndex= "1", FieldName= "Amplitude", FontName="Arial", FontSize=-15, Color="0", FontOptions=65936, FormatFactor=15.907776 ) 

		# Add a DC voltage source  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "VDC", "VDC2" , AREA = [350, 470, 370, 520], DIRECTION = 0, PAGE=0, XFLIP=0, _OPTIONS_=16, PORTS=[360, 470, 360, 520] , Amplitude = "12" , Lseries = "0" , Rseries = "0" ) 
		p1.PsimCreateNewElement(sch, "TEXTLINK", "", AREA = [370, 487, 401, 502], DIRECTION = 0, PAGE=0, XFLIP=0 , _OPTIONS_=0 , OWNERINDEX=nCreatedIndex, FieldIndex= "1", FieldName= "Amplitude", FontName="Arial", FontSize=-15, Color="0", FontOptions=65936, FormatFactor=15.907776 ) 

		# Add a Ground  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "Ground", "Ground" , AREA = [410, 460, 430, 480], DIRECTION = 0, PAGE=0, XFLIP=0, _OPTIONS_=16, PORTS=[420, 460] ) 

		# Add a Label  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "LABEL", "V+", DIRECTION = 0, PORTS=[360, 400], PAGE=0 , XFLIP=0, _OPTIONS_=16 ) 

		# Add a Label  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "LABEL", "V-", DIRECTION = 0, PORTS=[360, 520], PAGE=0 , XFLIP=0, _OPTIONS_=16 ) 

		# Add a Label  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "LABEL", "V+", DIRECTION = 90, PORTS=[530, 270], PAGE=0 , XFLIP=0, _OPTIONS_=16 ) 

		# Add a Label  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "LABEL", "V+", DIRECTION = 90, PORTS=[770, 280], PAGE=0 , XFLIP=0, _OPTIONS_=16 ) 

		# Add a Label  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "LABEL", "V+", DIRECTION = 90, PORTS=[1030, 290], PAGE=0 , XFLIP=0, _OPTIONS_=16 ) 

		# Add a Label  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "LABEL", "V+", DIRECTION = 90, PORTS=[1270, 300], PAGE=0 , XFLIP=0, _OPTIONS_=16 ) 

		# Add a Label  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "LABEL", "V-", DIRECTION = 270, PORTS=[530, 350], PAGE=0 , XFLIP=0, _OPTIONS_=16 ) 

		# Add a Label  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "LABEL", "V-", DIRECTION = 270, PORTS=[770, 320], PAGE=0 , XFLIP=0, _OPTIONS_=16 ) 

		# Add a Label  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "LABEL", "V-", DIRECTION = 270, PORTS=[1030, 330], PAGE=0 , XFLIP=0, _OPTIONS_=16 ) 

		# Add a Label  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "LABEL", "V-", DIRECTION = 270, PORTS=[1270, 340], PAGE=0 , XFLIP=0, _OPTIONS_=16 ) 
		# Add a wire  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "WIRE", "", PAGE=0 , X1="440" , Y1="240" , X2="390" , Y2="240" , X3="390" , Y3="280"  ) 

		# Add a wire  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "WIRE", "", PAGE=0 , X1="380" , Y1="280" , X2="390" , Y2="280"  ) 

		# Add a wire  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "WIRE", "", PAGE=0 , X1="390" , Y1="280" , X2="400" , Y2="280"  ) 

		# Add a wire  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "WIRE", "", PAGE=0 , X1="460" , Y1="290" , X2="460" , Y2="280"  ) 

		# Add a wire  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "WIRE", "", PAGE=0 , X1="450" , Y1="280" , X2="460" , Y2="280"  ) 

		# Add a wire  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "WIRE", "", PAGE=0 , X1="460" , Y1="280" , X2="500" , Y2="280"  ) 

		# Add a wire  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "WIRE", "", PAGE=0 , X1="500" , Y1="300" , X2="490" , Y2="300" , X3="490" , Y3="320" , X4="560" , Y4="320" , X5="560" , Y5="290"  ) 

		# Add a wire  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "WIRE", "", PAGE=0 , X1="550" , Y1="290" , X2="560" , Y2="290"  ) 

		# Add a wire  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "WIRE", "", PAGE=0 , X1="460" , Y1="350" , X2="460" , Y2="340"  ) 

		# Add a wire  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "WIRE", "", PAGE=0 , X1="490" , Y1="240" , X2="560" , Y2="240" , X3="560" , Y3="290"  ) 

		# Add a wire  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "WIRE", "", PAGE=0 , X1="680" , Y1="250" , X2="630" , Y2="250" , X3="630" , Y3="290"  ) 

		# Add a wire  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "WIRE", "", PAGE=0 , X1="620" , Y1="290" , X2="630" , Y2="290"  ) 

		# Add a wire  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "WIRE", "", PAGE=0 , X1="630" , Y1="290" , X2="640" , Y2="290"  ) 

		# Add a wire  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "WIRE", "", PAGE=0 , X1="700" , Y1="300" , X2="700" , Y2="290"  ) 

		# Add a wire  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "WIRE", "", PAGE=0 , X1="690" , Y1="290" , X2="700" , Y2="290"  ) 

		# Add a wire  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "WIRE", "", PAGE=0 , X1="700" , Y1="290" , X2="740" , Y2="290"  ) 

		# Add a wire  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "WIRE", "", PAGE=0 , X1="740" , Y1="310" , X2="730" , Y2="310" , X3="730" , Y3="330" , X4="800" , Y4="330" , X5="800" , Y5="300"  ) 

		# Add a wire  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "WIRE", "", PAGE=0 , X1="790" , Y1="300" , X2="800" , Y2="300"  ) 

		# Add a wire  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "WIRE", "", PAGE=0 , X1="700" , Y1="360" , X2="700" , Y2="350"  ) 

		# Add a wire  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "WIRE", "", PAGE=0 , X1="730" , Y1="250" , X2="800" , Y2="250" , X3="800" , Y3="300"  ) 

		# Add a wire  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "WIRE", "", PAGE=0 , X1="570" , Y1="290" , X2="560" , Y2="290"  ) 

		# Add a wire  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "WIRE", "", PAGE=0 , X1="960" , Y1="310" , X2="960" , Y2="300"  ) 

		# Add a wire  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "WIRE", "", PAGE=0 , X1="950" , Y1="300" , X2="960" , Y2="300"  ) 

		# Add a wire  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "WIRE", "", PAGE=0 , X1="960" , Y1="300" , X2="1000" , Y2="300"  ) 

		# Add a wire  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "WIRE", "", PAGE=0 , X1="960" , Y1="370" , X2="960" , Y2="360"  ) 

		# Add a wire  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "WIRE", "", PAGE=0 , X1="940" , Y1="260" , X2="890" , Y2="260" , X3="890" , Y3="300"  ) 

		# Add a wire  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "WIRE", "", PAGE=0 , X1="880" , Y1="300" , X2="890" , Y2="300"  ) 

		# Add a wire  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "WIRE", "", PAGE=0 , X1="890" , Y1="300" , X2="900" , Y2="300"  ) 

		# Add a wire  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "WIRE", "", PAGE=0 , X1="990" , Y1="260" , X2="1060" , Y2="260" , X3="1060" , Y3="310"  ) 

		# Add a wire  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "WIRE", "", PAGE=0 , X1="1050" , Y1="310" , X2="1060" , Y2="310"  ) 

		# Add a wire  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "WIRE", "", PAGE=0 , X1="1000" , Y1="320" , X2="990" , Y2="320" , X3="990" , Y3="340" , X4="1060" , Y4="340" , X5="1060" , Y5="310"  ) 

		# Add a wire  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "WIRE", "", PAGE=0 , X1="1200" , Y1="320" , X2="1200" , Y2="310"  ) 

		# Add a wire  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "WIRE", "", PAGE=0 , X1="1190" , Y1="310" , X2="1200" , Y2="310"  ) 

		# Add a wire  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "WIRE", "", PAGE=0 , X1="1200" , Y1="310" , X2="1240" , Y2="310"  ) 

		# Add a wire  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "WIRE", "", PAGE=0 , X1="1200" , Y1="380" , X2="1200" , Y2="370"  ) 

		# Add a wire  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "WIRE", "", PAGE=0 , X1="1180" , Y1="270" , X2="1130" , Y2="270" , X3="1130" , Y3="310"  ) 

		# Add a wire  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "WIRE", "", PAGE=0 , X1="1120" , Y1="310" , X2="1130" , Y2="310"  ) 

		# Add a wire  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "WIRE", "", PAGE=0 , X1="1130" , Y1="310" , X2="1140" , Y2="310"  ) 

		# Add a wire  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "WIRE", "", PAGE=0 , X1="1230" , Y1="270" , X2="1300" , Y2="270"  ) 

		# Add a wire  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "WIRE", "", PAGE=0 , X1="1300" , Y1="270" , X2="1300" , Y2="320"  ) 

		# Add a wire  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "WIRE", "", PAGE=0 , X1="1290" , Y1="320" , X2="1300" , Y2="320"  ) 

		# Add a wire  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "WIRE", "", PAGE=0 , X1="1240" , Y1="330" , X2="1230" , Y2="330" , X3="1230" , Y3="350" , X4="1300" , Y4="350" , X5="1300" , Y5="320"  ) 

		# Add a wire  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "WIRE", "", PAGE=0 , X1="800" , Y1="300" , X2="830" , Y2="300"  ) 

		# Add a wire  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "WIRE", "", PAGE=0 , X1="1070" , Y1="310" , X2="1060" , Y2="310"  ) 

		# Add a wire  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "WIRE", "", PAGE=0 , X1="310" , Y1="290" , X2="310" , Y2="280" , X3="330" , Y3="280"  ) 

		# Add a wire  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "WIRE", "", PAGE=0 , X1="310" , Y1="350" , X2="310" , Y2="340"  ) 

		# Add a wire  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "WIRE", "", PAGE=0 , X1="1310" , Y1="320" , X2="1300" , Y2="320"  ) 

		# Add a wire  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "WIRE", "", PAGE=0 , X1="420" , Y1="460" , X2="360" , Y2="460"  ) 

		# Add a wire  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "WIRE", "", PAGE=0 , X1="360" , Y1="450" , X2="360" , Y2="460"  ) 

		# Add a wire  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "WIRE", "", PAGE=0 , X1="360" , Y1="460" , X2="360" , Y2="470"  ) 

		# Add a wire  
		nCreatedIndex = p1.PsimCreateNewElement(sch, "WIRE", "", PAGE=0 , X1="530" , Y1="350" , X2="530" , Y2="310"  ) 

		#Save the circuit and close file handle 
		p1.PsimFileSave(sch, schematic_filePath)
		sch = None
	p1 = None

