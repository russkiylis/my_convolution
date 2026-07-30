insert into df_conv (
    data_type,
    min_angle_h,
    max_angle_h,
    step_h,
    conv_h_real,
    quality_h,
    min_angle_v,
    max_angle_v,
    step_v,
    conv_v_real,
    quality_v)
values ('real',
        :min_angle_h,
        :max_angle_h,
        :step_h,
        CAST(:conv_h_real AS REAL []),
        :quality_h,
        :min_angle_v,
        :max_angle_v,
        :step_v,
        CAST(:conv_v_real AS REAL []),
        :quality_v)
returning id;