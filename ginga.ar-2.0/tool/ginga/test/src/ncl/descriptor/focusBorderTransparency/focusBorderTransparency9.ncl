<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_descriptor_focusBorderTransparency_9" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="reg" width="360" height="288"/>
			<region id="reg2" />
		</regionBase>
		<descriptorBase>
			<descriptor id="desc" region="reg" focusIndex="1" focusBorderTransparency="0.5" focusBorderWidth="3"/>
			<descriptor id="desc2" region="reg2" />
		</descriptorBase>
	</head>
	<body>
		<port id="port" component="image" />
		<port id="port2" component="image2" />
		<media id="image" src="../../../../images/blue.jpg" descriptor="desc" />
		<media id="image2" src="../../../../images/red.jpg" descriptor="desc2" />
	</body>
</ncl>
