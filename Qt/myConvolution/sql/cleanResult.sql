-- Нерабочий мусор, ибо df_result свзязана с df_conv через foreign key и она удаляется через
-- TRUNCATE df_conv CASCADE;
TRUNCATE df_result;