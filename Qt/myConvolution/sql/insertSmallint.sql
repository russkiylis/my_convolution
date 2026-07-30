insert into df_conv (
    data_type,
    min_angle_h,
    max_angle_h,
    step_h,
    conv_h_smallint,
    quality_h,
    min_angle_v,
    max_angle_v,
    step_v,
    conv_v_smallint,
    quality_v)
values ('smallint',
        :min_angle_h,
        :max_angle_h,
        :step_h,
        CAST(:conv_h_smallint AS SMALLINT []),
        :quality_h,
        :min_angle_v,
        :max_angle_v,
        :step_v,
        CAST(:conv_v_smallint AS SMALLINT []),
        :quality_v)
returning id;