"""
Some stuff to play around with the MAXIM/DALLAS One Wire CRC-8.
"""

def crc_update(crc, data):
	crc ^= data
	for i in range(8):
		if crc & 1:
			crc = (crc >> 1) ^ 0x8c
		else:
			crc = (crc >> 1)
	return crc

def calc(data):
	"""Calculates crc of data (must be iterable)"""
	c = 0
	for b in data:
		c = crc_update(c, b)
	return c

def unhex(s):
	"""Converts stuff like "ff0042fefebeef" to byte strings"""
	d = b""
	for i in range(0, len(s), 2):
		d += bytes((int(s[i:i+2], 16), ))
	return d

def crccat(vec):
	"""Usefull for checking the testvectors from the crc catalogue"""
	return 0 == calc(unhex(re.sub(r"[^0-9a-fA-F]", "", vec)))

