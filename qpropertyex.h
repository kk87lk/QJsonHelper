#ifndef QPROPERTYEX_H
#define QPROPERTYEX_H

#define Q_PROPERTY_EX(type, name)							\
	public:													\
	Q_PROPERTY(type name READ get_##name WRITE set_##name)	\
	public:													\
    type get_##name() const { return m_##name; }			\
	void set_##name(type value) {m_##name = value; }		\
    protected:												\
	type m_##name;

//配合QJsonHelper使用，设置接口中自动保存配置文件
#define Q_PROPERTY_EX_AUTOSAVE(type, name)                  \
    public:													\
    Q_PROPERTY(type name READ get_##name WRITE set_##name)	\
    public:													\
    type get_##name() const { return m_##name; }			\
    void set_##name(type value)                 \
    {                                           \
        if (m_##name != value)                  \
        {                                       \
            m_##name = value;                   \
            if (isLoadFinish())                 \
                QJsonHelper::save(m_filePath);  \
        }                                       \
    }                                           \
    protected:                                  \
    type m_##name;

#define MEMBER(type, name, init)							\
    public:													\
    type get_##name() const { return m_##name; }			\
    void set_##name(type value) {m_##name = value; }		\
    protected:												\
    type m_##name = init;

#define PROPERTY(type, name)							\
    public:													\
    type get_##name() const { return m_##name; }			\
    void set_##name(type value) {m_##name = value; }		\
    protected:												\
    type m_##name;

#endif // QPROPERTYEX_H
