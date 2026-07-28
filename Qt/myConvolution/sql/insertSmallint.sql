insert into df_conv (
    data_type,
    min_angle_h,
    max_angle_h,
    step_h,
    conv_h_smallint,
    min_angle_v,
    max_angle_v,
    step_v,
    conv_v_smallint)
values ('smallint',
        :min_angle_h,
        :max_angle_h,
        :step_h,
        CAST(:conv_h_smallint AS SMALLINT []),
        :min_angle_v,
        :max_angle_v,
        :step_v,
        CAST(:conv_v_smallint AS SMALLINT []));