<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_region_left_1" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="regA" >
				<region id="regC">
					<region id="regD"/>
				</region>
				<region id="regE" />	
			</region>
		</regionBase>
		<descriptorBase>
			<descriptor id="desc" region="regD" />
		</descriptorBase>
	</head>
	<body>
		<port id="port" component="image" />
		<media id="image" src="../../../../images/blue.jpg" descriptor="desc" />
	</body>
</ncl>