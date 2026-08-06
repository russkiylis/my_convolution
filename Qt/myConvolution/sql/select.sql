SELECT
    result.id,
    result.result_timestamp,
    result.sysname,
    result.azimuth,
    result.elevation,
    result.power,
    result.frequency,
    result.latitude,
    result.longitude,
    conv.data_type,
    conv.quality,
    conv.count_h,
    conv.count_v,
    conv.conv
FROM df_result AS result
         JOIN df_conv AS conv USING (id)
ORDER BY result.result_timestamp DESC, result.id DESC;