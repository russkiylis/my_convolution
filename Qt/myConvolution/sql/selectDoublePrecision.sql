SELECT
    array_to_json(conv_h_double)::text AS conv_h,
    array_to_json(conv_v_double)::text AS conv_v
FROM df_conv
WHERE id = :id;
