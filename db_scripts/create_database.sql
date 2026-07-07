-- Creating database
CREATE DATABASE my_convolution;

-- Switching to database
\c my_convolution

-- Creating table df_conv
CREATE TABLE IF NOT EXISTS df_conv (
	id BIGINT GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
	min_angle SMALLINT  NOT NULL DEFAULT 0 CHECK (min_angle >= 0 AND min_angle < 360),
	max_angle SMALLINT  NOT NULL DEFAULT 360 CHECK (max_angle > 0 AND max_angle <= 360),
	step NUMERIC(2,1) NOT NULL DEFAULT 0.1 CHECK (step IN (1.0, 0.5, 0.25, 0.1)),
	data REAL [] NOT NULL CHECK (ARRAY_LENGTH(data,1) = ((max_angle-min_angle)/step))
);

-- Creating table df_result
CREATE TABLE IF NOT EXISTS df_result (
	id BIGINT  PRIMARY KEY
		REFERENCES df_conv(id)
		ON DELETE CASCADE,
	result_timestamp TIMESTAMPTZ NOT NULL DEFAULT NOW(),
	azimuth	NUMERIC(6,3) NOT NULL CHECK (azimuth >= 0 AND azimuth <= 360),
	power NUMERIC(6,3) NOT NULL,
	frequency BIGINT NOT NULL CHECK (frequency > 0),
	longitude NUMERIC(9,6) CHECK (longitude >= -180 AND longitude <= 180),
	latitude NUMERIC(8,6) CHECK (latitude >= -90 AND latitude <= 90),
	sysname VARCHAR(20)
);
