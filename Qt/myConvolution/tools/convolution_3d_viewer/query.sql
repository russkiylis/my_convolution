-- Лёгкий каталог всех сохранённых свёрток без загрузки тяжёлого BYTEA.
-- Сама выбранная свёртка запрашивается просмотрщиком отдельно по result.id.
SELECT
    result.id,
    result.result_timestamp,
    result.sysname,
    result.azimuth::double precision AS azimuth,
    result.elevation::double precision AS elevation,
    result.power::double precision AS power,
    result.frequency::double precision AS frequency,
    result.latitude::double precision AS latitude,
    result.longitude::double precision AS longitude,

    conv.data_type::text AS data_type,
    conv.quality::double precision AS quality,
    conv.min_angle_h::double precision AS min_angle_h,
    conv.max_angle_h::double precision AS max_angle_h,
    conv.step_h::double precision AS step_h,
    conv.count_h::integer AS count_h,
    conv.min_angle_v::double precision AS min_angle_v,
    conv.max_angle_v::double precision AS max_angle_v,
    conv.step_v::double precision AS step_v,
    conv.count_v::integer AS count_v,
    octet_length(conv.conv)::bigint AS conv_bytes,

    row_number() OVER (
        PARTITION BY result.sysname, conv.data_type
        ORDER BY result.result_timestamp DESC NULLS LAST, result.id DESC
    )::bigint AS history_position,
    count(*) OVER (
        PARTITION BY result.sysname, conv.data_type
    )::bigint AS history_count
FROM public.df_result AS result
JOIN public.df_conv AS conv ON conv.id = result.id
WHERE conv.conv IS NOT NULL
  AND conv.count_h > 0
  AND conv.count_v > 0
ORDER BY
    result.sysname NULLS LAST,
    conv.data_type::text,
    result.result_timestamp DESC NULLS LAST,
    result.id DESC;
