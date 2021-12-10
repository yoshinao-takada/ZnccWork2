#ifndef APPDATASET_H_
#define APPDATASET_H_
#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief AppDataSet defines multiple instances of input dataset by
 * their file or directory paths.
 * 
 */
struct AppDataSet;
typedef struct AppDataSet AppDataSet_t, *pAppDataSet_t;
typedef const AppDataSet_t *pcAppDataSet_t;

/**
 * @brief create a new object from file defining a dataset
 * 
 * @param filePath [in] dataset configuration
 * @return pcAppDataSet_t pointer to a new object
 */
pcAppDataSet_t AppDataSet_NewFromFile(const char* filePath);

/**
 * @brief delete an existing object
 * 
 * @param dataset 
 */
void AppDataSet_Delete(pcAppDataSet_t dataset);

int AppDataSet_Count(pcAppDataSet_t dataset);

const char* AppDataSet_Path(pcAppDataSet_t dataset, int index);
#ifdef __cplusplus
}
#endif
#endif /* APPDATASET_H_ */