-- Creating user stc and database stc, granting privileges
CREATE USER stc WITH PASSWORD :'db_password';
CREATE DATABASE stc OWNER stc;
GRANT ALL PRIVILEGES ON DATABASE stc to stc;

-- Connecting to db via stc instead of postgres
\connect stc

CREATE TABLE df_conv (
	id BIGINT GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
	min_angle NUMERIC(6,3) NOT NULL DEFAULT 0 CHECK (min_angle >= 0 AND min_angle < 360),
	max_angle NUMERIC(6,3) NOT NULL DEFAULT 360 CHECK (max_angle > 0 AND max_angle <= 360),
	step NUMERIC(2,1) NOT NULL DEFAULT 0.1 CHECK (step IN (1.0, 0.5, 0.1)),
	data REAL [] NOT NULL
);

CREATE TABLE df_result (
	id BIGINT  PRIMARY KEY
		REFERENCES df_conf(id)
		ON DELETE CASCADE,
	result_timestamp TIMESTAMPZ NOT NULL DEFAULT NOW(),
	azimuth	NUMERIC(6,3) NOT NULL CHECK (azimuth >= 0 AND azimuth <= 360),
	power NUMERIC(6,3) NOT NULL,
	frequency BIGINT NOT NULL CHECK (frequency > 0),
	longitude NUMERIC(9,6) CHECK (longitude >= -180 AND longitude <= 180),
	latitude NUMERIC(8,6) CHECK (latitude >= -90 AND latitude <= 90),
	sysname VARCHAR(20)
);
