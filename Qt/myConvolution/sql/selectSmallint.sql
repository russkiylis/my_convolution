SELECT
    array_to_json(conv_h_smallint)::text AS conv_h,
    array_to_json(conv_v_smallint)::text AS conv_v
FROM df_conv
WHERE id = :id;