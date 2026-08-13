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