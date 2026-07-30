CREATE TABLE IF NOT EXISTS df_conv (
                                       id BIGINT GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
                                       data_type public.data_type NOT NULL,
                                       min_angle_h SMALLINT  NOT NULL DEFAULT 0 CHECK (min_angle_h >= 0 AND min_angle_h < 360),
                                       max_angle_h SMALLINT  NOT NULL DEFAULT 360 CHECK (max_angle_h > 0 AND max_angle_h <= 360),
                                       step_h NUMERIC(3,2) NOT NULL DEFAULT 0.1 CHECK (step_h IN (1.0, 0.5, 0.2, 0.1, 0.01)),
                                       conv_h_double DOUBLE PRECISION [],
                                       conv_h_real REAL [],
                                       conv_h_smallint SMALLINT [],
                                       quality_h REAL,
                                       min_angle_v SMALLINT  NOT NULL DEFAULT -45 CHECK (min_angle_v >= -45 AND min_angle_v < 45),
                                       max_angle_v SMALLINT  NOT NULL DEFAULT 45 CHECK (max_angle_v > -45 AND max_angle_v <= 45),
                                       step_v NUMERIC(3,2) NOT NULL DEFAULT 0.1 CHECK (step_v IN (1.0, 0.5, 0.2, 0.1, 0.01)),
                                       conv_v_double DOUBLE PRECISION [],
                                       conv_v_real REAL [],
                                       conv_v_smallint SMALLINT [],
                                       quality_v REAL,

                                       CONSTRAINT df_conv_angle_h_range_check
                                           CHECK (min_angle_h < max_angle_h),

                                       CONSTRAINT df_conv_angle_v_range_check
                                           CHECK (min_angle_v < max_angle_v)
);