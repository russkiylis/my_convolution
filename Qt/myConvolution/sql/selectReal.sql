SELECT
    array_to_json(conv_h_real)::text AS conv_h,
    array_to_json(conv_v_real)::text AS conv_v
FROM df_conv
WHERE id = :id;