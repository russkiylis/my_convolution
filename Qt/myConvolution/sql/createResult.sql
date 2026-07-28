CREATE TABLE IF NOT EXISTS df_result (
                                         id BIGINT  PRIMARY KEY
                                             REFERENCES df_conv(id)
                                                 ON DELETE CASCADE,
                                         result_timestamp TIMESTAMPTZ NOT NULL DEFAULT NOW(),
                                         azimuth	NUMERIC(6,3) NOT NULL CHECK (azimuth >= 0 AND azimuth <= 360),
                                         elevation NUMERIC(6,3) NOT NULL CHECK (elevation >= -45 AND elevation <= 45),
                                         power NUMERIC(6,3) NOT NULL,
                                         frequency NUMERIC(15,2) NOT NULL CHECK (frequency >= 0),
                                         longitude NUMERIC(9,6) CHECK (longitude >= -180 AND longitude <= 180),
                                         latitude NUMERIC(8,6) CHECK (latitude >= -90 AND latitude <= 90),
                                         sysname VARCHAR(25)
);