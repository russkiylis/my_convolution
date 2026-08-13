-- Creating database
CREATE DATABASE my_convolution;

-- Switching to database
\c my_convolution

-- Creating enum data type
CREATE TYPE public.data_type AS ENUM (
    'double_le',
    'double_be',
    'real_le',
    'real_be',
    'smallint_le',
    'smallint_be',
    'pa_8b_le',
    'pa_8b_be',
    'pa_7b_le',
    'pa_7b_be',
    'pa_6b_le',
    'pa_6b_be',
    'pa_5b_le',
    'pa_5b_be',
    'pa_4b_le',
    'pa_4b_be',
    'pa_3b_le',
    'pa_3b_be',
    'pa_2b_le',
    'pa_2b_be'
    );

-- Creating enum byte order
CREATE TYPE public.byte_order AS ENUM (
    'big_endian',
    'little_endian'
    );

-- Creating table df_conv
CREATE TABLE IF NOT EXISTS df_conv (
	id BIGINT GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
	data_type public.data_type NOT NULL,
	min_angle_h SMALLINT  NOT NULL DEFAULT 0 CHECK (min_angle_h >= 0 AND min_angle_h < 360),
	max_angle_h SMALLINT  NOT NULL DEFAULT 360 CHECK (max_angle_h > 0 AND max_angle_h <= 360),
	step_h NUMERIC(3,2) NOT NULL DEFAULT 0.1 CHECK (step_h IN (5.0, 2.0, 1.0, 0.5, 0.2, 0.1)),
	count_h SMALLINT,
	min_angle_v SMALLINT  NOT NULL DEFAULT -45 CHECK (min_angle_v >= -45 AND min_angle_v < 45),
	max_angle_v SMALLINT  NOT NULL DEFAULT 45 CHECK (max_angle_v > -45 AND max_angle_v <= 45),
	step_v NUMERIC(3,2) NOT NULL DEFAULT 0.1 CHECK (step_v IN (5.0, 2.0, 1.0, 0.5, 0.2, 0.1)),
	count_v SMALLINT,
	quality REAL,
	conv BYTEA,

	CONSTRAINT df_conv_angle_h_range_check
		CHECK (min_angle_h < max_angle_h),

	CONSTRAINT df_conv_angle_v_range_check
		CHECK (min_angle_v < max_angle_v)
);

-- Creating table df_result
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

-- Creating function for getting convolution metadata
create or replace function get_conv_metadata(target_id bigint)
returns table (
    conv_id bigint,
    data_type public.data_type,
    byte_order public.byte_order,
    bits_per_item smallint,
    byte_count int
              )
language plpgsql
as $$
    begin
        return query
        select
            c.id,           -- id свёртки
            c.data_type,    -- Тип записи данных

            -- Выбираем последовательность
            case
                when c.data_type::text like '%_be' then 'big_endian'
                else 'little_endian'
            end::public.byte_order as byte_order,

            -- Выбираем количество бит на символ
            case
                when c.data_type::text like 'double_%' then 64
                when c.data_type::text like 'real_%' then 32
                when c.data_type::text like 'smallint_%' then 16
                when c.data_type::text like 'pa_8b_%' then 8
                when c.data_type::text like 'pa_7b_%' then 7
                when c.data_type::text like 'pa_6b_%' then 6
                when c.data_type::text like 'pa_5b_%' then 5
                when c.data_type::text like 'pa_4b_%' then 4
                when c.data_type::text like 'pa_3b_%' then 3
                when c.data_type::text like 'pa_2b_%' then 2
            end::smallint as bits_per_item,

            octet_length(c.conv) as byte_count -- Длина свёртки в байтах

        from df_conv c
        where id = target_id;
    end;
$$;
